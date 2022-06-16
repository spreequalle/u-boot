/*
 * Copyright (C) 2014-2016 bodhi <mibodhi@gmail.com>
 * Based on
 *
 * Copyright (C) 2012
 * David Purdy <david.c.purdy@gmail.com>
 *
 * Based on Kirkwood support:
 * (C) Copyright 2009
 * Marvell Semiconductor <www.marvell.com>
 * Written-by: Prafulla Wadaskar <prafulla@marvell.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _CONFIG_POGOPLUGV4_H
#define _CONFIG_POGOPLUGV4_H

/*
 * Machine type definition and ID
 */
#define MACH_TYPE_POGOPLUGV4		3960
#define CONFIG_MACH_TYPE		MACH_TYPE_POGOPLUGV4

/*
 * High Level Configuration Options (easy to change)
 */
#define CONFIG_FEROCEON_88FR131		/* #define CPU Core subversion */
#define CONFIG_KW88F6192		/* SOC Name */
#define CONFIG_SKIP_LOWLEVEL_INIT	/* disable board lowlevel_init */

#define CONFIG_FEATURE_COMMAND_EDITING
#define CONFIG_SYS_64BIT_LBA

/*
 * Commands configuration
 */

#define CONFIG_KIRKWOOD_GPIO
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
	"usb reset ; " \
	"fatload usb 0:1 0x2000000 initramfs.bin ; "\
	"bootm 0x2000000 ; " \
	"ubi part ubi; " \
	"ubi read 0x800000 kernel; " \
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
#define CONFIG_NETCONSOLE
#define CONFIG_MVGBE_PORTS	{1, 0}	/* enable port 0 only */
#define CONFIG_PHY_BASE_ADR	0
#endif /* CONFIG_CMD_NET */

/*
 * File system
 */
#define CONFIG_JFFS2_NAND
#define CONFIG_JFFS2_LZO

/*
 * SATA
 */
#ifdef CONFIG_MVSATA_IDE
#define CONFIG_SYS_ATA_IDE0_OFFSET	MV_SATA_PORT0_OFFSET
#endif

/*
 *  Date Time
 */
#ifdef CONFIG_CMD_DATE
#define CONFIG_RTC_MV
#endif /* CONFIG_CMD_DATE */

/*
 * Kirkwood GPIO
 */
#define CONFIG_KIRKWOOD_GPIO

#endif /* _CONFIG_POGOPLUGV4_H */
