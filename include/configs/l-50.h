/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (C) 2020
 * Pawel Dembicki <paweldembicki@gmail.com>
 *
 * Based on Kirkwood support:
 * (C) Copyright 2009
 * Marvell Semiconductor <www.marvell.com>
 * Written-by: Prafulla Wadaskar <prafulla@marvell.com>
 */

#ifndef _CONFIG_L50_H
#define _CONFIG_L50_H

#include "openwrt-kirkwood-common.h"

/*
 * High Level Configuration Options (easy to change)
 */
#define CONFIG_FEROCEON_88FR131		/* CPU Core subversion */
#define CONFIG_KW88F6281		/* SOC Name */
#define CONFIG_SKIP_LOWLEVEL_INIT	/* disable board lowlevel_init */

/*
 * mv-common.h should be defined after CMD configs since it used them
 * to enable certain macros
 */
#include "mv-common.h"

/* Remove or override few declarations from mv-common.h */

/*
 * Ethernet Driver configuration
 */
#ifdef CONFIG_CMD_NET
#define CONFIG_MVGBE_PORTS		{1, 1} /* enable port 0 only */
#define CONFIG_NETCONSOLE
#endif

#define CONFIG_MV88E61XX_CPU_PORT_RX_DELAY
#define CONFIG_MV88E61XX_CPU_PORT_TX_DELAY

/*
 * Enable GPI0 support
 */
#define CONFIG_KIRKWOOD_GPIO

/*
 * Default environment variables
 */
#define CONFIG_BOOTCOMMAND \
	"ubi part ubi; " \
	"ubi read 0x800000 kernel; " \
	"bootm 0x800000"

#define CONFIG_EXTRA_ENV_SETTINGS \
	"bootargs=console=ttyS0,115200\0"	\
	"mtdids=" CONFIG_MTDIDS_DEFAULT "\0"	\
	"mtdparts=" CONFIG_MTDPARTS_DEFAULT "\0"	\
	"bootargs_root=\0"
#endif /* _CONFIG_L50_H */
