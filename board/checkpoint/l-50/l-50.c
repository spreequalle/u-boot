// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2020
 * Pawel Dembicki <paweldembicki@gmail.com>
 *
 * Based on Kirkwood support:
 * (C) Copyright 2009
 * Marvell Semiconductor <www.marvell.com>
 * Written-by: Prafulla Wadaskar <prafulla@marvell.com>
 */

#include <common.h>
#include <dm.h>
#include <i2c.h>
#include <miiphy.h>
#include <netdev.h>
#include <asm/arch/cpu.h>
#include <asm/arch/soc.h>
#include <asm/arch/mpp.h>
#include <asm/arch/gpio.h>
#include "l-50.h"

DECLARE_GLOBAL_DATA_PTR;

int board_early_init_f(void)
{
	/* Gpio configuration */
	mvebu_config_gpio(L50_OE_VAL_LOW, L50_OE_VAL_HIGH,
			  L50_OE_LOW, L50_OE_HIGH);

	/* Multi-Purpose Pins Functionality configuration */
	static const u32 kwmpp_config[] = {
		MPP0_NF_IO2,
		MPP1_NF_IO3,
		MPP2_NF_IO4,
		MPP3_NF_IO5,
		MPP4_NF_IO6,
		MPP5_NF_IO7,
		MPP6_SYSRST_OUTn,
		MPP7_SPI_SCn,
		MPP8_TW_SDA,
		MPP9_TW_SCK,
		MPP10_UART0_TXD,
		MPP11_UART0_RXD,
		MPP12_SD_CLK,
		MPP13_SD_CMD,
		MPP14_SD_D0,
		MPP15_SD_D1,
		MPP16_SD_D2,
		MPP17_SD_D3,
		MPP18_NF_IO0,
		MPP19_NF_IO1,
		MPP20_GE1_0,
		MPP21_GE1_1,
		MPP22_GE1_2,
		MPP23_GE1_3,
		MPP24_GE1_4,
		MPP25_GE1_5,
		MPP26_GE1_6,
		MPP27_GE1_7,
		MPP28_GPIO,
		MPP29_GPIO,
		MPP30_GE1_10,
		MPP31_GE1_11,
		MPP32_GE1_12,
		MPP33_GE1_13,
		MPP34_GPIO,
		MPP35_GPIO,
		MPP36_AUDIO_SPDIFI,	/* value from stock u-boot */
		MPP37_GPIO,
		MPP38_GPIO,
		MPP39_TDM_SPI_CS0,
		MPP40_GPIO,
		MPP41_GPIO,
		MPP42_TDM_SPI_MOSI,
		MPP43_TDM_CODEC_INTn,
		MPP44_GPIO,
		MPP45_TDM_PCLK,
		MPP46_GPIO,
		MPP47_TDM_DRX,
		MPP48_GPIO,
		MPP49_GPIO,
		0
	};
	kirkwood_mpp_conf(kwmpp_config, NULL);

	return 0;
}

void board_gpio_expander_init(void)
{
	struct udevice *dev0, *dev1;
	uchar data_buffer;
	int ret;

	ret = i2c_get_chip_for_busnum(0, L50_GPIO0_I2C_ADDRESS, 1, &dev0);
	if (ret) {
		debug("%s: Cannot find I2C GPIO expander chip 0x02%X\n",
		      __func__, L50_GPIO0_I2C_ADDRESS);
		return;
	}

	ret = i2c_get_chip_for_busnum(0, L50_GPIO1_I2C_ADDRESS, 1, &dev1);
	if (ret) {
		debug("%s: Cannot find I2C GPIO expander chip 0x02%X\n",
		      __func__, L50_GPIO1_I2C_ADDRESS);
		return;
	}

	/* Set IO as output */
	data_buffer = 0x0;
	dm_i2c_write(dev0, 1, &data_buffer, 1);
	dm_i2c_write(dev1, 1, &data_buffer, 1);

	/* Set all leds off, reset asserted, pwr off */
	data_buffer = 0xbf;
	dm_i2c_write(dev0, 0, &data_buffer, 1);
	data_buffer = 0x1c;
	dm_i2c_write(dev1, 0, &data_buffer, 1);

	mdelay(100);

	/* Set pwr on */
	data_buffer = 0xa5;
	dm_i2c_write(dev1, 0, &data_buffer, 1);

	mdelay(100);

	/* Set reset deasserted, status red led enabled*/
	data_buffer = 0xff;
	dm_i2c_write(dev0, 0, &data_buffer, 1);
	data_buffer = 0xe3;
	dm_i2c_write(dev1, 0, &data_buffer, 1);
}

int board_init(void)
{
	/* Boot parameters address */
	gd->bd->bi_boot_params = mvebu_sdram_bar(0) + 0x100;

	board_gpio_expander_init();

	return 0;
}

#ifdef CONFIG_RESET_PHY_R
/* Configure and initialize PHY */
void reset_phy(void)
{
	u16 devadr;
	char *name = "ethernet-controller@72000";

	if (miiphy_set_current_dev(name))
		return;

	/* command to read PHY dev address */
	if (miiphy_read(name, 0xEE, 0xEE, (u16 *)&devadr)) {
		printf("Err..(%s) could not read PHY dev address\n", __func__);
		return;
	}

	/*
	 * Fix PHY led configuration
	 */
	miiphy_write(name, devadr, MV88E1116_PGADR_REG, 3);
	miiphy_write(name, devadr, 0x10, 0x1177);
	miiphy_write(name, devadr, 0x11, 0x4417);
	miiphy_write(name, devadr, MV88E1116_PGADR_REG, 0);

	debug("88E1116 Initialized on %s\n", name);
}
#endif /* CONFIG_RESET_PHY_R */
