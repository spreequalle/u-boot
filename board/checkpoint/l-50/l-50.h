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

#ifndef __L50_H
#define __L50_H

/* GPIO configuration */
#define L50_OE_LOW			0x30000000
#define L50_OE_HIGH			0x0000004c
#define L50_OE_VAL_LOW		0x00000000
#define L50_OE_VAL_HIGH		0x00000000

/* Expander GPIO addresses */

#define L50_GPIO0_I2C_ADDRESS		0x20
#define L50_GPIO1_I2C_ADDRESS		0x21

/* PHY register */
#define MV88E1116_PGADR_REG		22

#endif /* __L50_H */
