/*
 * Copyright 2016 Freescale Semiconductor, Inc.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <asm/io.h>
#include <netdev.h>
#include <fm_eth.h>
#include <fsl_dtsec.h>
#include <fsl_mdio.h>
#include <malloc.h>
#include "../../freescale/common/fman.h"
#include "dragonfruit.h"
#include <i2c.h>

static int scalys_carrier_init_rgmii_phy0(void);
static int scalys_carrier_init_rgmii_phy1(void);

static int scalys_carrier_init_rgmii_phy0()
{
	u32 val;
	struct ccsr_gpio *pgpio2 = (void *)(GPIO2_BASE_ADDR);
	
	 /* Reset RGMII PHY 0 by setting ETH1_RESET_N_3V3 high */
	val = in_be32(&pgpio2->gpdir) | ETH1_RESET_N_3V3;
	out_be32(&pgpio2->gpdir, val);
	
	val = in_be32(&pgpio2->gpdat);
	val &= ~ETH1_RESET_N_3V3;
	out_be32(&pgpio2->gpdat, val);
	
	/* Wait for 10 ms to to meet reset timing */
	
        mdelay(10);
	
	val = in_be32(&pgpio2->gpdat) | ETH1_RESET_N_3V3;
	out_be32(&pgpio2->gpdat, val);
	
	/* Write 0x4111 to reg 0x18 on both PHYs to change LEDs usage */
	miiphy_write("FSL_MDIO0",0,0x18,0x4111);

	return 0;
}

static int scalys_carrier_init_rgmii_phy1()
{
	int ret = 0;
	uint8_t i2c_data;
	
        /* Reset RGMII PHY 1 by setting ETH2_RESET_N_3V3 high */
	i2c_set_bus_num(0);
	ret = i2c_read(MOD_I2C_IO_ADDR, 0x0E, 1, &i2c_data, 1);
	i2c_data &= ~ETH2_RESET_N_3V3;
	ret = i2c_write(MOD_I2C_IO_ADDR, 0x0E, 1, &i2c_data, 1);
        ret = i2c_read(MOD_I2C_IO_ADDR, 0x06, 1, &i2c_data, 1);
        i2c_data &= ~ETH2_RESET_N_3V3;
        ret = i2c_write(MOD_I2C_IO_ADDR, 0x06, 1, &i2c_data, 1);
        
        /* Wait for 10 ms to to meet reset timing */
        mdelay(10);
        
        ret = i2c_read(MOD_I2C_IO_ADDR, 0x06, 1, &i2c_data, 1);
        i2c_data |= ETH2_RESET_N_3V3;
        ret = i2c_write(MOD_I2C_IO_ADDR, 0x06, 1, &i2c_data, 1);
	
	/* Write 0x4111 to reg 0x18 on both PHYs to change LEDs usage */
	miiphy_write("FSL_MDIO0",1,0x18,0x4111);
	
	return ret;
}

int board_eth_init(bd_t *bis)
{
#ifdef CONFIG_FMAN_ENET
	int i;
	struct memac_mdio_info dtsec_mdio_info;
	struct mii_dev *dev;
	u32 srds_s1;
        
	struct ccsr_gur *gur = (void *)(CONFIG_SYS_FSL_GUTS_ADDR);

	srds_s1 = in_be32(&gur->rcwsr[4]) &
			FSL_CHASSIS2_RCWSR4_SRDS1_PRTCL_MASK;
	srds_s1 >>= FSL_CHASSIS2_RCWSR4_SRDS1_PRTCL_SHIFT;

	dtsec_mdio_info.regs =
		(struct memac_mdio_controller *)CONFIG_SYS_FM1_DTSEC_MDIO_ADDR;

	dtsec_mdio_info.name = DEFAULT_FM_MDIO_NAME;

	/* Register the 1G MDIO bus */
	fm_memac_mdio_init(bis, &dtsec_mdio_info);
	
	/* Reset PHY's and do some initial mdio configuration */
	scalys_carrier_init_rgmii_phy0();
	scalys_carrier_init_rgmii_phy1();

        mdelay(100);

	/* Set the two RGMII PHY addresses */
	fm_info_set_phy_address(FM1_DTSEC3, RGMII_PHY1_ADDR);
	fm_info_set_phy_address(FM1_DTSEC4, RGMII_PHY2_ADDR);
    
	/* Disable SGMII related ports for now. 
	   These ports can be implemented if needed. */
	fm_disable_port(FM1_DTSEC5);
	fm_disable_port(FM1_DTSEC6);
	
	/* In case SERDES1[PRTCL] = 0x3333 then disable the following 
	   SGMII ports. These ports are not usable on default carrier board */
	fm_disable_port(FM1_DTSEC9);
	fm_disable_port(FM1_DTSEC10);
	
	/* In case SERDES1[PRTCL] = 0x1133 then disable XFI ports
	   In current hardware they aren't usable since the mdio is NC */
	fm_disable_port(FM1_10GEC1);
	fm_disable_port(FM1_10GEC2);

	dev = miiphy_get_dev_by_name(DEFAULT_FM_MDIO_NAME);
	for (i = FM1_DTSEC1; i < FM1_DTSEC1 + CONFIG_SYS_NUM_FM1_DTSEC; i++) {
		fm_info_set_mdio(i, dev);
	}

	cpu_eth_init(bis);
#endif

	return pci_eth_init(bis);
}
