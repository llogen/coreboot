/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <acpi/acpi_gnvs.h>
#include <acpi/acpigen.h>
#include <arch/ioapic.h>
#include <arch/smp/mpspec.h>
#include <cbmem.h>
#include <device/pci_ops.h>
#include <cpu/x86/smm.h>
#include <console/console.h>
#include <types.h>
#include <string.h>
#include <arch/cpu.h>
#include <cpu/x86/msr.h>
#include <cpu/intel/turbo.h>
#include <ec/google/chromeec/ec.h>
#include <vendorcode/google/chromeos/gnvs.h>
#include <soc/acpi.h>
#include <soc/cpu.h>
#include <soc/iomap.h>
#include <soc/lpc.h>
#include <soc/msr.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>
#include <soc/systemagent.h>
#include <soc/intel/broadwell/chip.h>

/*
 * List of supported C-states in this processor. Only the ULT parts support C8,
 * C9, and C10.
 */
enum {
	C_STATE_C0,             /* 0 */
	C_STATE_C1,             /* 1 */
	C_STATE_C1E,            /* 2 */
	C_STATE_C3,             /* 3 */
	C_STATE_C6_SHORT_LAT,   /* 4 */
	C_STATE_C6_LONG_LAT,    /* 5 */
	C_STATE_C7_SHORT_LAT,   /* 6 */
	C_STATE_C7_LONG_LAT,    /* 7 */
	C_STATE_C7S_SHORT_LAT,  /* 8 */
	C_STATE_C7S_LONG_LAT,   /* 9 */
	C_STATE_C8,             /* 10 */
	C_STATE_C9,             /* 11 */
	C_STATE_C10,            /* 12 */
	NUM_C_STATES
};

#define MWAIT_RES(state, sub_state)                         \
	{                                                   \
		.addrl = (((state) << 4) | (sub_state)),    \
		.space_id = ACPI_ADDRESS_SPACE_FIXED,       \
		.bit_width = ACPI_FFIXEDHW_VENDOR_INTEL,    \
		.bit_offset = ACPI_FFIXEDHW_CLASS_MWAIT,    \
		.access_size = ACPI_FFIXEDHW_FLAG_HW_COORD, \
	}

static acpi_cstate_t cstate_map[NUM_C_STATES] = {
	[C_STATE_C0] = { },
	[C_STATE_C1] = {
		.latency = 0,
		.power = 1000,
		.resource = MWAIT_RES(0, 0),
	},
	[C_STATE_C1E] = {
		.latency = 0,
		.power = 1000,
		.resource = MWAIT_RES(0, 1),
	},
	[C_STATE_C3] = {
		.latency = C_STATE_LATENCY_FROM_LAT_REG(0),
		.power = 900,
		.resource = MWAIT_RES(1, 0),
	},
	[C_STATE_C6_SHORT_LAT] = {
		.latency = C_STATE_LATENCY_FROM_LAT_REG(1),
		.power = 800,
		.resource = MWAIT_RES(2, 0),
	},
	[C_STATE_C6_LONG_LAT] = {
		.latency = C_STATE_LATENCY_FROM_LAT_REG(2),
		.power = 800,
		.resource = MWAIT_RES(2, 1),
	},
	[C_STATE_C7_SHORT_LAT] = {
		.latency = C_STATE_LATENCY_FROM_LAT_REG(1),
		.power = 700,
		.resource = MWAIT_RES(3, 0),
	},
	[C_STATE_C7_LONG_LAT] = {
		.latency = C_STATE_LATENCY_FROM_LAT_REG(2),
		.power = 700,
		.resource = MWAIT_RES(3, 1),
	},
	[C_STATE_C7S_SHORT_LAT] = {
		.latency = C_STATE_LATENCY_FROM_LAT_REG(1),
		.power = 700,
		.resource = MWAIT_RES(3, 2),
	},
	[C_STATE_C7S_LONG_LAT] = {
		.latency = C_STATE_LATENCY_FROM_LAT_REG(2),
		.power = 700,
		.resource = MWAIT_RES(3, 3),
	},
	[C_STATE_C8] = {
		.latency = C_STATE_LATENCY_FROM_LAT_REG(3),
		.power = 600,
		.resource = MWAIT_RES(4, 0),
	},
	[C_STATE_C9] = {
		.latency = C_STATE_LATENCY_FROM_LAT_REG(4),
		.power = 500,
		.resource = MWAIT_RES(5, 0),
	},
	[C_STATE_C10] = {
		.latency = C_STATE_LATENCY_FROM_LAT_REG(5),
		.power = 400,
		.resource = MWAIT_RES(6, 0),
	},
};

static int cstate_set_s0ix[3] = {
	C_STATE_C1E,
	C_STATE_C7S_LONG_LAT,
	C_STATE_C10
};

static int cstate_set_non_s0ix[3] = {
	C_STATE_C1E,
	C_STATE_C3,
	C_STATE_C7S_LONG_LAT
};

static int get_cores_per_package(void)
{
	struct cpuinfo_x86 c;
	struct cpuid_result result;
	int cores = 1;

	get_fms(&c, cpuid_eax(1));
	if (c.x86 != 6)
		return 1;

	result = cpuid_ext(0xb, 1);
	cores = result.ebx & 0xff;

	return cores;
}

static acpi_tstate_t tss_table_fine[] = {
	{ 100, 1000, 0, 0x00, 0 },
	{ 94, 940, 0, 0x1f, 0 },
	{ 88, 880, 0, 0x1e, 0 },
	{ 82, 820, 0, 0x1d, 0 },
	{ 75, 760, 0, 0x1c, 0 },
	{ 69, 700, 0, 0x1b, 0 },
	{ 63, 640, 0, 0x1a, 0 },
	{ 57, 580, 0, 0x19, 0 },
	{ 50, 520, 0, 0x18, 0 },
	{ 44, 460, 0, 0x17, 0 },
	{ 38, 400, 0, 0x16, 0 },
	{ 32, 340, 0, 0x15, 0 },
	{ 25, 280, 0, 0x14, 0 },
	{ 19, 220, 0, 0x13, 0 },
	{ 13, 160, 0, 0x12, 0 },
};

static acpi_tstate_t tss_table_coarse[] = {
	{ 100, 1000, 0, 0x00, 0 },
	{ 88, 875, 0, 0x1f, 0 },
	{ 75, 750, 0, 0x1e, 0 },
	{ 63, 625, 0, 0x1d, 0 },
	{ 50, 500, 0, 0x1c, 0 },
	{ 38, 375, 0, 0x1b, 0 },
	{ 25, 250, 0, 0x1a, 0 },
	{ 13, 125, 0, 0x19, 0 },
};

static void generate_T_state_entries(int core, int cores_per_package)
{
	/* Indicate SW_ALL coordination for T-states */
	acpigen_write_TSD_package(core, cores_per_package, SW_ALL);

	/* Indicate FFixedHW so OS will use MSR */
	acpigen_write_empty_PTC();

	/* Set a T-state limit that can be modified in NVS */
	acpigen_write_TPC("\\TLVL");

	/*
	 * CPUID.(EAX=6):EAX[5] indicates support
	 * for extended throttle levels.
	 */
	if (cpuid_eax(6) & (1 << 5))
		acpigen_write_TSS_package(
			ARRAY_SIZE(tss_table_fine), tss_table_fine);
	else
		acpigen_write_TSS_package(
			ARRAY_SIZE(tss_table_coarse), tss_table_coarse);
}

static void generate_C_state_entries(void)
{
	acpi_cstate_t map[3];
	int *set;
	int i;

	config_t *config = config_of_soc();

	if (config->s0ix_enable)
		set = cstate_set_s0ix;
	else
		set = cstate_set_non_s0ix;

	for (i = 0; i < 3; i++) {
		memcpy(&map[i], &cstate_map[set[i]], sizeof(acpi_cstate_t));
		map[i].ctype = i + 1;
	}

	/* Generate C-state tables */
	acpigen_write_CST_package(map, ARRAY_SIZE(map));
}

static int calculate_power(int tdp, int p1_ratio, int ratio)
{
	u32 m;
	u32 power;

	/*
	 * M = ((1.1 - ((p1_ratio - ratio) * 0.00625)) / 1.1) ^ 2
	 *
	 * Power = (ratio / p1_ratio) * m * tdp
	 */

	m = (110000 - ((p1_ratio - ratio) * 625)) / 11;
	m = (m * m) / 1000;

	power = ((ratio * 100000 / p1_ratio) / 100);
	power *= (m / 100) * (tdp / 1000);
	power /= 1000;

	return (int)power;
}

static void generate_P_state_entries(int core, int cores_per_package)
{
	int ratio_min, ratio_max, ratio_turbo, ratio_step;
	int coord_type, power_max, power_unit, num_entries;
	int ratio, power, clock, clock_max;
	msr_t msr;

	/* Determine P-state coordination type from MISC_PWR_MGMT[0] */
	msr = rdmsr(MSR_MISC_PWR_MGMT);
	if (msr.lo & MISC_PWR_MGMT_EIST_HW_DIS)
		coord_type = SW_ANY;
	else
		coord_type = HW_ALL;

	/* Get bus ratio limits and calculate clock speeds */
	msr = rdmsr(MSR_PLATFORM_INFO);
	ratio_min = (msr.hi >> (40-32)) & 0xff; /* Max Efficiency Ratio */

	/* Determine if this CPU has configurable TDP */
	if (cpu_config_tdp_levels()) {
		/* Set max ratio to nominal TDP ratio */
		msr = rdmsr(MSR_CONFIG_TDP_NOMINAL);
		ratio_max = msr.lo & 0xff;
	} else {
		/* Max Non-Turbo Ratio */
		ratio_max = (msr.lo >> 8) & 0xff;
	}
	clock_max = ratio_max * CPU_BCLK;

	/* Calculate CPU TDP in mW */
	msr = rdmsr(MSR_PKG_POWER_SKU_UNIT);
	power_unit = 2 << ((msr.lo & 0xf) - 1);
	msr = rdmsr(MSR_PKG_POWER_SKU);
	power_max = ((msr.lo & 0x7fff) / power_unit) * 1000;

	/* Write _PCT indicating use of FFixedHW */
	acpigen_write_empty_PCT();

	/* Write _PPC with no limit on supported P-state */
	acpigen_write_PPC_NVS();

	/* Write PSD indicating configured coordination type */
	acpigen_write_PSD_package(core, 1, coord_type);

	/* Add P-state entries in _PSS table */
	acpigen_write_name("_PSS");

	/* Determine ratio points */
	ratio_step = PSS_RATIO_STEP;
	num_entries = (ratio_max - ratio_min) / ratio_step;
	while (num_entries > PSS_MAX_ENTRIES-1) {
		ratio_step <<= 1;
		num_entries >>= 1;
	}

	/* P[T] is Turbo state if enabled */
	if (get_turbo_state() == TURBO_ENABLED) {
		/* _PSS package count including Turbo */
		acpigen_write_package(num_entries + 2);

		msr = rdmsr(MSR_TURBO_RATIO_LIMIT);
		ratio_turbo = msr.lo & 0xff;

		/* Add entry for Turbo ratio */
		acpigen_write_PSS_package(
			clock_max + 1,		/*MHz*/
			power_max,		/*mW*/
			PSS_LATENCY_TRANSITION,	/*lat1*/
			PSS_LATENCY_BUSMASTER,	/*lat2*/
			ratio_turbo << 8,	/*control*/
			ratio_turbo << 8);	/*status*/
	} else {
		/* _PSS package count without Turbo */
		acpigen_write_package(num_entries + 1);
	}

	/* First regular entry is max non-turbo ratio */
	acpigen_write_PSS_package(
		clock_max,		/*MHz*/
		power_max,		/*mW*/
		PSS_LATENCY_TRANSITION,	/*lat1*/
		PSS_LATENCY_BUSMASTER,	/*lat2*/
		ratio_max << 8,		/*control*/
		ratio_max << 8);	/*status*/

	/* Generate the remaining entries */
	for (ratio = ratio_min + ((num_entries - 1) * ratio_step);
	     ratio >= ratio_min; ratio -= ratio_step) {

		/* Calculate power at this ratio */
		power = calculate_power(power_max, ratio_max, ratio);
		clock = ratio * CPU_BCLK;

		acpigen_write_PSS_package(
			clock,			/*MHz*/
			power,			/*mW*/
			PSS_LATENCY_TRANSITION,	/*lat1*/
			PSS_LATENCY_BUSMASTER,	/*lat2*/
			ratio << 8,		/*control*/
			ratio << 8);		/*status*/
	}

	/* Fix package length */
	acpigen_pop_len();
}

void generate_cpu_entries(const struct device *device)
{
	int coreID, cpuID, pcontrol_blk = ACPI_BASE_ADDRESS, plen = 6;
	int totalcores = dev_count_cpu();
	int cores_per_package = get_cores_per_package();
	int numcpus = totalcores/cores_per_package;

	printk(BIOS_DEBUG, "Found %d CPU(s) with %d core(s) each.\n",
	       numcpus, cores_per_package);

	for (cpuID = 1; cpuID <= numcpus; cpuID++) {
		for (coreID = 1; coreID <= cores_per_package; coreID++) {
			if (coreID > 1) {
				pcontrol_blk = 0;
				plen = 0;
			}

			/* Generate processor \_SB.CPUx */
			acpigen_write_processor(
				(cpuID - 1) * cores_per_package+coreID - 1,
				pcontrol_blk, plen);

			/* Generate P-state tables */
			generate_P_state_entries(
				coreID - 1, cores_per_package);

			/* Generate C-state tables */
			generate_C_state_entries();

			/* Generate T-state tables */
			generate_T_state_entries(
				cpuID - 1, cores_per_package);

			acpigen_pop_len();
		}
	}

	/* PPKG is usually used for thermal management
	   of the first and only package. */
	acpigen_write_processor_package("PPKG", 0, cores_per_package);

	/* Add a method to notify processor nodes */
	acpigen_write_processor_cnot(cores_per_package);
}