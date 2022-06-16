/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (C) 2012
 * David Purdy <david.c.purdy@gmail.com>
 *
 * Based on Kirkwood support:
 * (C) Copyright 2009
 * Marvell Semiconductor <www.marvell.com>
 * Written-by: Prafulla Wadaskar <prafulla@marvell.com>
 */

#ifndef _CONFIG_POGO_E02_H
#define _CONFIG_POGO_E02_H

/*
 * Machine type definition and ID
 */
#define CONFIG_MACH_TYPE		MACH_TYPE_POGO_E02

/*
 * High Level Configuration Options (easy to change)
 */
#define CONFIG_FEROCEON_88FR131		/* CPU Core subversion */
#define CONFIG_KW88F6281		/* SOC Name */
#define CONFIG_SKIP_LOWLEVEL_INIT	/* disable board lowlevel_init */

/*
 * Commands configuration
 */

/*
 * mv-common.h should be defined after CMD configs since it used them
 * to enable certain macros
 */
#include "mv-common.h"

/*
 *  Environment variables configurations
 */

/*
 * Default environment variables
 */
#define CONFIG_BOOTCOMMAND \
	"setenv bootargs ${console} ${mtdparts} ${bootargs_root}; "	\
	"ubi part ubi; "						\
	"ubifsmount ubi:rootfs; "					\
	"ubi read 0x800000 kernel; "					\
	"bootm 0x800000"

#define CONFIG_EXTRA_ENV_SETTINGS \
	"console=console=ttyS0,115200\0"	\
	"mtdids=nand0=orion_nand\0"		\
	"mtdparts="CONFIG_MTDPARTS_DEFAULT "\0"	\
	"bootargs_root=\0"

/*
 * Ethernet Driver configuration
 */
#ifdef CONFIG_CMD_NET
#define CONFIG_MVGBE_PORTS	{1, 0}	/* enable port 0 only */
#define CONFIG_PHY_BASE_ADR	0
#endif /* CONFIG_CMD_NET */

/*
 * File system
 */

#include "openwrt-kirkwood-common.h"

#endif /* _CONFIG_POGO_E02_H */
