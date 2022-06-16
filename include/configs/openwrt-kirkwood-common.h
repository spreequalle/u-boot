/*
 * Copyright (C) 2013 Luka Perkov <luka@openwrt.org>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __OPENWRT_KIRKWOOD_COMMON_H
#define __OPENWRT_KIRKWOOD_COMMON_H

/* Ethernet */
#if defined(CONFIG_CMD_NET)
#define CONFIG_SERVERIP		192.168.1.2
#define CONFIG_IPADDR		192.168.1.1
#endif

/* second stage loader */
#if defined(CONFIG_SECOND_STAGE)
#undef CONFIG_ENV_IS_IN_NAND
#undef CONFIG_ENV_SECT_SIZE
#define CONFIG_ENV_IS_NOWHERE
#endif

/* Various */
#define CONFIG_BZIP2

/* Unnecessary */
#undef CONFIG_BOOTM_NETBSD
#undef CONFIG_BOOTM_PLAN9
#undef CONFIG_BOOTM_RTEMS

#endif /* __OPENWRT_KIRKWOOD_COMMON_H */
