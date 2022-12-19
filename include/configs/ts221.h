/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * (C) Copyright 2022 Erik Hoppe <generik@spreequalle.de>
 */

#ifndef _CONFIG_TS211_H
#define _CONFIG_TS211_H

/*
 * High Level Configuration Options (easy to change)
 */
#define CONFIG_FEROCEON_88FR131 1       /* CPU Core subversion */
#define CONFIG_KW88F6281        1       /* SOC Name */
#define CONFIG_SKIP_LOWLEVEL_INIT       /* disable board lowlevel_init */

#define CONFIG_KIRKWOOD_EGIGA_INIT      /* Enable GbePort0/1 for kernel */
#define CONFIG_KIRKWOOD_PCIE_INIT       /* Enable PCIE Port0 */
#define CONFIG_KIRKWOOD_RGMII_PAD_1V8   /* Set RGMII Pad voltage to 1.8V */

/*
 * Commands configuration
 */
#define CONFIG_PREBOOT

/*
 * mv-common.h should be defined after CMD configs since it used them
 * to enable certain macros
 */
#include "mv-common.h"

/*
 * Default environment variables
 */
#define CONFIG_BOOTCOMMAND \
        "ubi part ubi; " \
        "ubi read 0x800000 kernel; " \
        "bootm 0x800000"

#define CONFIG_EXTRA_ENV_SETTINGS \
        "console=console=ttyS0,115200\0"        \
        "mtdids=nand0=orion_nand\0"             \
        "mtdparts="CONFIG_MTDPARTS_DEFAULT "\0" \
        "bootargs_root=\0"

#define CONFIG_SYS_SATA_MAX_DEVICE	2
#define CONFIG_SYS_ATA_BASE_ADDR MV_SATA_BASE
#define CONFIG_IDE_PREINIT
#define CONFIG_SYS_ATA_IDE0_OFFSET      MV_SATA_PORT0_OFFSET
#define CONFIG_SYS_ATA_IDE1_OFFSET      MV_SATA_PORT1_OFFSET

/*
 * Ethernet Driver configuration
 */
#ifdef CONFIG_CMD_NET
#define CONFIG_MVGBE_PORTS              {1, 0}  /* enable port 0 only */
#define CONFIG_PHY_BASE_ADR             0x1
#define CONFIG_NETCONSOLE
#endif /* CONFIG_CMD_NET */

/*
 * File system
 */
#define CONFIG_JFFS2_NAND
#define CONFIG_JFFS2_LZO

/*
 *  Date Time
 */
#ifdef CONFIG_CMD_DATE
#define CONFIG_RTC_MV
#endif /* CONFIG_CMD_DATE */

#define CONFIG_KIRKWOOD_GPIO /* Enable GPIO Support */

#endif /* _CONFIG_TS211_H */
