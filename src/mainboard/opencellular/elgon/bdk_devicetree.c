/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

// This file is automatically generated.
// DO NOT EDIT BY HAND.

#include <bdk-devicetree.h>

const struct bdk_devicetree_key_value devtree[] = {
{"DDR-CONFIG-DQX-CTL", "0x4"},
{"DDR-CONFIG-WODT-MASK.RANKS2.DIMMS2", "0xc0c0303"},
{"DDR-CONFIG-WODT-MASK.RANKS4.DIMMS1", "0x1030203"},
{"DDR-CONFIG-MODE1-RTT-WR.RANKS1.DIMMS1.RANK0", "0x4"},
{"DDR-CONFIG-MODE1-RTT-WR.RANKS1.DIMMS2.RANK0", "0x4"},
{"DDR-CONFIG-MODE1-RTT-WR.RANKS1.DIMMS2.RANK2", "0x4"},
{"DDR-CONFIG-MODE1-RTT-WR.RANKS2.DIMMS1.RANK0", "0x2"},
{"DDR-CONFIG-MODE1-RTT-WR.RANKS2.DIMMS1.RANK1", "0x2"},
{"DDR-CONFIG-MODE1-RTT-WR.RANKS2.DIMMS2", "0x2"},
{"DDR-CONFIG-MODE1-RTT-WR.RANKS4", "0x1"},
{"DDR-CONFIG-MODE1-DIC.RANKS4.DIMMS1", "0x1"},
{"DDR-CONFIG-MODE1-RTT-NOM.RANKS2.DIMMS2", "0x2"},
{"DDR-CONFIG-MODE1-RTT-NOM.RANKS4.DIMMS1.RANK0", "0x4"},
{"DDR-CONFIG-MODE1-RTT-NOM.RANKS4.DIMMS1.RANK2", "0x4"},
{"DDR-CONFIG-MODE2-RTT-PARK.RANKS1.DIMMS1.RANK0", "0x1"},
{"DDR-CONFIG-MODE2-RTT-PARK.RANKS1.DIMMS2.RANK0", "0x5"},
{"DDR-CONFIG-MODE2-RTT-PARK.RANKS1.DIMMS2.RANK2", "0x5"},
{"DDR-CONFIG-MODE2-RTT-PARK.RANKS2.DIMMS1.RANK0", "0x2"},
{"DDR-CONFIG-MODE2-RTT-PARK.RANKS2.DIMMS1.RANK1", "0x2"},
{"DDR-CONFIG-MODE2-RTT-PARK.RANKS2.DIMMS2", "0x1"},
{"DDR-CONFIG-MODE2-RTT-PARK.RANKS4.DIMMS1.RANK0", "0x6"},
{"DDR-CONFIG-MODE2-RTT-PARK.RANKS4.DIMMS1.RANK1", "0x6"},
{"DDR-CONFIG-MODE2-VREF-VALUE.RANKS1.DIMMS1.RANK0", "0x22"},
{"DDR-CONFIG-MODE2-VREF-VALUE.RANKS1.DIMMS2.RANK0", "0x1f"},
{"DDR-CONFIG-MODE2-VREF-VALUE.RANKS1.DIMMS2.RANK2", "0x1f"},
{"DDR-CONFIG-MODE2-VREF-VALUE.RANKS2.DIMMS1.RANK0", "0x19"},
{"DDR-CONFIG-MODE2-VREF-VALUE.RANKS2.DIMMS1.RANK1", "0x19"},
{"DDR-CONFIG-MODE2-VREF-VALUE.RANKS2.DIMMS2", "0x19"},
{"DDR-CONFIG-MODE2-VREF-VALUE.RANKS4.DIMMS1.RANK0", "0x1f"},
{"DDR-CONFIG-MODE2-VREF-VALUE.RANKS4.DIMMS1.RANK1", "0x1f"},
{"DDR-CONFIG-RODT-CTL.RANKS1.DIMMS1", "0x7"},
{"DDR-CONFIG-RODT-CTL.RANKS1.DIMMS2", "0x3"},
{"DDR-CONFIG-RODT-CTL.RANKS2.DIMMS1", "0x3"},
{"DDR-CONFIG-RODT-CTL.RANKS2.DIMMS2", "0x7"},
{"DDR-CONFIG-RODT-CTL.RANKS4.DIMMS1", "0x7"},
{"DDR-CONFIG-RODT-MASK.RANKS2.DIMMS2", "0x4080102"},
{"DDR-CONFIG-RODT-MASK.RANKS4.DIMMS1", "0x1010202"},
{"DDR-CONFIG-CUSTOM-MIN-RTT-NOM-IDX", "0x1"},
{"DDR-CONFIG-CUSTOM-MAX-RTT-NOM-IDX", "0x7"},
{"DDR-CONFIG-CUSTOM-MIN-RODT-CTL", "0x1"},
{"DDR-CONFIG-CUSTOM-MAX-RODT-CTL", "0x7"},
{"DDR-CONFIG-CUSTOM-OFFSET-EN", "0x1"},
{"DDR-CONFIG-CUSTOM-OFFSET", "0x2"},
{"DDR-CONFIG-CUSTOM-DDR2T", "0x1"},
{"DDR-CONFIG-CUSTOM-MAXIMUM-ADJACENT-RLEVEL-DELAY-INCREMENT", "0x2"},
{"DDR-CONFIG-CUSTOM-FPRCH2", "0x2"},
{"PHY-ADDRESS.N0.BGX0.P0", "0x00001000"},
{"PHY-ADDRESS.N0.BGX2.P0", "0x00001000"},
{"BGX-ENABLE.N0.BGX0.P0", "1"},
{"BGX-ENABLE.N0.BGX0.P1", "0"},
{"BGX-ENABLE.N0.BGX0.P2", "0"},
{"BGX-ENABLE.N0.BGX0.P3", "0"},
{"BGX-ENABLE.N0.BGX1.P0", "0"},
{"BGX-ENABLE.N0.BGX1.P1", "0"},
{"BGX-ENABLE.N0.BGX2.P0", "1"},
{"BDK-NUM-PACKET-BUFFERS", "0x1000"},
{"BDK-PACKET-BUFFER-SIZE", "0x400"},
{"BDK-SHOW-LINK-STATUS", "1"},
{"BDK-COREMASK", "0"},
{"BDK-BOOT-MENU-TIMEOUT", "0x05"},
{"BDK-BOOT-PATH-OPTION", "0"},
{"BDK-CONFIG-TRACE", "1"},
{"MULTI-NODE", "0"},
{"QLM-AUTO-CONFIG", "0"},
{"QLM-DIP-AUTO-CONFIG", "0"},
{"QLM-MODE.N0.QLM0", "PCIE_1X1"},
{"QLM-MODE.N0.QLM1", "SGMII_2X1"},
{"QLM-MODE.N0.QLM2", "PCIE_1X2"},
{"QLM-MODE.N0.QLM3", "SATA_2X1"},
{"QLM-FREQ.N0.QLM0", "6000"},
{"QLM-FREQ.N0.QLM1", "1250"},
{"QLM-FREQ.N0.QLM2", "6000"},
{"QLM-FREQ.N0.QLM3", "6000"},
{"QLM-CLK.N0.QLM0", "2"},
{"QLM-CLK.N0.QLM1", "2"},
{"QLM-CLK.N0.QLM2", "2"},
{"QLM-CLK.N0.QLM3", "2"},
{"DDR-SPEED.N0", "1600"},
{"DDR-CONFIG-SPD-ADDR.DIMM0.LMC0", "0x1050"},
{"USB-REFCLK-SRC.N0.PORT0", "0"},
{"GPIO-PIN-SELECT-GPIO45", "0x2"},
{"GPIO-PIN-SELECT-GPIO46", "0x250"},
{0, 0},
};
