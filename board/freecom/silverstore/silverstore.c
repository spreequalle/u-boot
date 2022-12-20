// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2022
 * generik <generik@spreequalle.de>
 */

#include <common.h>
#include <init.h>
#include <net.h>
#include <asm/global_data.h>
#include <linux/bitops.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <miiphy.h>
#include <netdev.h>
#include <status_led.h>
#include <asm/arch/cpu.h>
#include <asm/arch/soc.h>
#include <asm/arch/mpp.h>
#include <asm/arch/gpio.h>

int board_early_init_f(void)
{
	return 0;
}

int board_init(void)
{
	return 0;
}

#ifdef CONFIG_RESET_PHY_R
/* automatically defined by kirkwood config.h */
void reset_phy(void)
{
}
#endif
