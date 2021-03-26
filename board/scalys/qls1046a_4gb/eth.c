/*
 * Copyright 2016 Freescale Semiconductor, Inc.
 *           2021 Scalys
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
static int scalys_carrier_init_sfp_phys(void);
static int scalys_carrier_init_retimer(void);

/* Finistar FCLF-85 configuration (AN2036) */
uint8_t sfp_phy_config[][2] = {
	{ 0x1b, 0x90 }, /* Enable SGMII mode */
	{ 0x1b, 0x84 },
	{ 0x09, 0x0F }, /* Adv. 1000BASE-T FD/HD */
	{ 0x09, 0x00 },
	{ 0x00, 0x81 }, /* Software reset */
	{ 0x00, 0x40 },
};

static int scalys_carrier_init_rgmii_phy0()
{
	int ret;
	u32 val;
	struct ccsr_gpio *pgpio2 = (void *)(GPIO2_BASE_ADDR);
	
	 /* Reset RGMII PHY 0 by setting ETH1_RESET_N_3V3 high */
	val = in_be32(&pgpio2->gpdir) | ETH1_RESET_N_3V3_GPIO;
	out_be32(&pgpio2->gpdir, val);

	val = in_be32(&pgpio2->gpdat);
	val &= ~ETH1_RESET_N_3V3_GPIO;
	out_be32(&pgpio2->gpdat, val);

	/* Wait for 10 ms to to meet reset timing */
	mdelay(10);

	val = in_be32(&pgpio2->gpdat) | ETH1_RESET_N_3V3_GPIO;
	out_be32(&pgpio2->gpdat, val);
	
	/* Write 0x4111 to reg 0x18 on both PHYs to change LEDs usage */
	ret = miiphy_write("FSL_MDIO0",0,0x18,0x4111);

	return ret;
}

static int scalys_carrier_init_rgmii_phy1()
{
	int ret = 0;
	uint8_t i2c_data;

	/* Reset RGMII PHY 1 by setting ETH2_RESET_N_3V3 high */
	i2c_set_bus_num(0);
	/* Set to output */
	ret = i2c_read(MOD_I2C_IO_ADDR, 0x0E, 1, &i2c_data, 1);
	if (ret) {
		return ret;
	}
	i2c_data &= ~ETH2_RESET_N_3V3_GPIO;
	ret = i2c_write(MOD_I2C_IO_ADDR, 0x0E, 1, &i2c_data, 1);
	if (ret) {
		return ret;
	}
	/* Set to low */
	ret = i2c_read(MOD_I2C_IO_ADDR, 0x06, 1, &i2c_data, 1);
	if (ret) {
		return ret;
	}
	i2c_data &= ~ETH2_RESET_N_3V3_GPIO;
	ret = i2c_write(MOD_I2C_IO_ADDR, 0x06, 1, &i2c_data, 1);
	if (ret) {
		return ret;
	}

	/* Wait for 10 ms to to meet reset timing */
	mdelay(10);

	/* Set to high */
	ret = i2c_read(MOD_I2C_IO_ADDR, 0x06, 1, &i2c_data, 1);
	if (ret) {
		return ret;
	}
	i2c_data |= ETH2_RESET_N_3V3_GPIO;
	ret = i2c_write(MOD_I2C_IO_ADDR, 0x06, 1, &i2c_data, 1);
	if (ret) {
		return ret;
	}

	/* Write 0x4111 to reg 0x18 on both PHYs to change LEDs usage */
	miiphy_write("FSL_MDIO0",1,0x18,0x4111);

	return ret;
}

static int scalys_carrier_init_sfp_phys()
{
	int ret = 0;
	uint8_t i2c_data;

	/* Turn SFPx LEDs off by default */
	/* It is up to the OS to control these if desired */
	i2c_set_bus_num(0);
	ret = i2c_read(MOD_I2C_IO_ADDR, 0x0E, 1, &i2c_data, 1);
	i2c_data &= ~(SGMII0_LINKSTAT_GPIO| \
					SGMII0_ACT_GPIO| \
					SGMII1_LINKSTAT_GPIO| \
					SGMII1_ACT_GPIO);
	ret = i2c_write(MOD_I2C_IO_ADDR, 0x0E, 1, &i2c_data, 1);
	ret = i2c_read(MOD_I2C_IO_ADDR, 0x06, 1, &i2c_data, 1);
	i2c_data &= ~(SGMII0_LINKSTAT_GPIO| \
					SGMII0_ACT_GPIO| \
					SGMII1_LINKSTAT_GPIO| \
					SGMII1_ACT_GPIO);
	ret = i2c_write(MOD_I2C_IO_ADDR, 0x06, 1, &i2c_data, 1);

	/* Remove SFP TX_disable */
	/* Set to output */
	ret = i2c_read(CAR_I2C_IO_ADDR, 0x0E, 1, &i2c_data, 1);
	if (ret) {
		return ret;
	}
	i2c_data &= ~TX_DISABLE_GPIO;
	ret = i2c_write(CAR_I2C_IO_ADDR, 0x0E, 1, &i2c_data, 1);
	if (ret) {
		return ret;
	}
	/* Set to low */
	ret = i2c_read(CAR_I2C_IO_ADDR, 0x06, 1, &i2c_data, 1);
	if (ret) {
		return ret;
	}
	i2c_data &= ~TX_DISABLE_GPIO;
	ret = i2c_write(CAR_I2C_IO_ADDR, 0x06, 1, &i2c_data, 1);
	if (ret) {
		return ret;
	}

	/* Wait until PHYs are ready */
	mdelay(100);

	i2c_set_bus_num(3);

	/* If inserted, the SFP modules have to be configured. (Finistar FCLF-85)
	*/
	for (int phy_addr=2; phy_addr<4; phy_addr++) {
		/* I2C multiplexer channel selection (SGMII/SFP is 2 (bottom left) & 3 (top left)) */
		i2c_data = (1 << phy_addr);
		ret = i2c_write(0x70, 0, 1, &i2c_data, 1);
		if (ret) {
			printf("Error Setting SFP i2c MUX\n");
			break;
		}

		ret = i2c_probe(0x56);
		if (ret) {
			printf("Warning: No SFP PHY module detected on slot %i.\n", phy_addr);
		} else {
			for (int i = 0; i < 6; i++) {
				ret = i2c_write(0x56, sfp_phy_config[i][0], 1, &(sfp_phy_config[i][1]), 1);
				if (ret) {
					printf("Error configuring SFP PHY on slot %i. (I2C Data: %d, Reg: %d).\n", phy_addr, sfp_phy_config[i][1], sfp_phy_config[i][0]);
					break;
				}
			}
		}
	}
	/* The SFP+ modules cannot be used and therefore are not configured. */
	return 0;
}

static int scalys_carrier_init_retimer()
{
	int ret;
	uint8_t i2c_data;

	/* Set EDC_RST_N_3V3 low to keep retimer in reset */
	i2c_set_bus_num(0);
	/* Set to output */
	ret = i2c_read(MOD_I2C_IO_ADDR, 0x0D, 1, &i2c_data, 1);
	if (ret) {
		return ret;
	}
	i2c_data &= ~(EDC_RST_N_3V3_GPIO);
	ret = i2c_write(MOD_I2C_IO_ADDR, 0x0D, 1, &i2c_data, 1);
	if (ret) {
		return ret;
	}

	/* Set to low */
	ret = i2c_read(MOD_I2C_IO_ADDR, 0x05, 1, &i2c_data, 1);
	if (ret) {
	return ret;
	}
	i2c_data &= ~(EDC_RST_N_3V3_GPIO);
	ret = i2c_write(MOD_I2C_IO_ADDR, 0x05, 1, &i2c_data, 1);
	if (ret) {
		return ret;
	}

	return 0;
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
	if(scalys_carrier_init_rgmii_phy0() != 0)
		printf("Initializing RGMII PHY 0 failed\n");
	if(scalys_carrier_init_rgmii_phy1() != 0)
		printf("Initializing RGMII PHY 1 failed\n");
	if(scalys_carrier_init_sfp_phys() != 0)
		printf("Initializing SFP PHYs failed\n");
	if(scalys_carrier_init_retimer() != 0)
		printf("Disabling XFI retimer failed\n");

	/* Wait until PHYs are ready */
	mdelay(100);

	/* Set the two RGMII PHY addresses */
	fm_info_set_phy_address(FM1_DTSEC3, RGMII_PHY1_ADDR);
	fm_info_set_phy_address(FM1_DTSEC4, RGMII_PHY2_ADDR);

	/* SGMII PHYs have no MDIO interface and have already been configured
	 * above through I2C */
	
	/* In case SERDES1[PRTCL] = 0x3333 then disable the following
	 * SGMII ports. These ports are not usable on default carrier board */
	fm_disable_port(FM1_DTSEC9);
	fm_disable_port(FM1_DTSEC10);
	
	/* In case SERDES1[PRTCL] = 0x1133 then disable XFI ports
	 * In current hardware they aren't usable since the mdio is NC */
	fm_disable_port(FM1_10GEC1);
	fm_disable_port(FM1_10GEC2);

	dev = miiphy_get_dev_by_name(DEFAULT_FM_MDIO_NAME);
	for (i = FM1_DTSEC1; i < FM1_DTSEC1 + CONFIG_SYS_NUM_FM1_DTSEC; i++) {
		/* Skip SGMII interfaces since their PHYs are not connected to the MDIO bus */
		if (fm_info_get_enet_if(i) == PHY_INTERFACE_MODE_SGMII) {
			continue;
		}
		fm_info_set_mdio(i, dev);
	}

	cpu_eth_init(bis);
#endif

	return pci_eth_init(bis);
}
