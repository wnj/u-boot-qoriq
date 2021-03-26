/*
 * Copyright 2021 Scalys B.V.
 * opensource@scalys.com
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include "dragonfruit.h"
#include <asm/io.h>
#include <i2c.h>

int scalys_carrier_init_pcie()
{
	uint8_t i2c_data;
	u32 val;
	struct ccsr_gpio *pgpio2 = (void *)(GPIO2_BASE_ADDR);

	/* Set SLOT4_REFCLK_OE_N_GPIO to low */
	i2c_set_bus_num(0);
	i2c_read(MOD_I2C_IO_ADDR, 0x0D, 1, &i2c_data, 1);
	i2c_data &= ~SLOT4_REFCLK_OE_N_GPIO;
	i2c_write(MOD_I2C_IO_ADDR, 0x0D, 1, &i2c_data, 1);
	i2c_read(MOD_I2C_IO_ADDR, 0x05, 1, &i2c_data, 1);
	i2c_data &= ~SLOT4_REFCLK_OE_N_GPIO;
	i2c_write(MOD_I2C_IO_ADDR, 0x05, 1, &i2c_data, 1);

	/* Set PEX_PERST_N_GPIO and PEX_REFCLK_SEL_GPIO to high */
	val = in_be32(&pgpio2->gpdir) | PEX_PERST_N_GPIO | PEX_REFCLK_SEL_GPIO;
	out_be32(&pgpio2->gpdir, val);
	val = in_be32(&pgpio2->gpdat) | PEX_PERST_N_GPIO;
	val &= ~PEX_REFCLK_SEL_GPIO;
	out_be32(&pgpio2->gpdat, val);

	return 0;
}

int scalys_carrier_setup_muxing()
{
	u32 val;
	struct ccsr_gpio *pgpio2 = (void *)(GPIO2_BASE_ADDR);

	/*
	 * Sets SERDES lanes as follows:
	 * SD1 D to SFP+ on X600 (via retimer) - unused for now
	 * SD1 C to SFP+ on X600 (via retimer) - unused for now
	 * SD1 B to SFP on X600 - unused for now
	 * SD1 A to SFP on X600 - unused for now
	 * SD2 A-D: 4x PCIe on X704
	 */
	val = in_be32(&pgpio2->gpdir) | MUX_SER0_1_SEL_GPIO | MUX_SER2_3_SEL_GPIO | MUX_SER5_6_SEL_GPIO;
	out_be32(&pgpio2->gpdir, val);
	val = in_be32(&pgpio2->gpdat) | MUX_SER0_1_SEL_GPIO | MUX_SER2_3_SEL_GPIO;
	val &= ~MUX_SER5_6_SEL_GPIO;
	out_be32(&pgpio2->gpdat, val);

	/* SERDES clock is already enabled */

	return 0;
}
