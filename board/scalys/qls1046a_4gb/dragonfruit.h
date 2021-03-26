/*
 * Copyright 2021 Scalys B.V.
 * opensource@scalys.com
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */


#ifndef _DRAGON_FRUIT_H
#define _DRAGON_FRUIT_H

#define GPIO_BIT_OFFSET(_BIT_)	(0x80000000u >> _BIT_)

/*
 * GPIO bank 2
 */
/* QSPI_B_DATA0/IFC_PAR0/GPIO2_13/FTM6_CH0 */
#define PEX_PERST_N_GPIO        GPIO_BIT_OFFSET(13)
/* QSPI_B_DATA1/IFC_PAR1/GPIO2_14/FTM6_CH1 */
#define PEX_REFCLK_SEL_GPIO     GPIO_BIT_OFFSET(14)
/* QSPI_B_DATA2/IFC_PERR_B/GPIO2_15/ FTM6_EXTCLK */
#define ETH1_RESET_N_3V3_GPIO   GPIO_BIT_OFFSET(15)
/* IFC_A25/GPIO2_25/ QSPI_A_DATA3/FTM5_CH0/ IFC_CS4_B/IFC_RB2_B */
#define MUX_SER0_1_SEL_GPIO     GPIO_BIT_OFFSET(25)
/* IFC_A26/GPIO2_26/ FTM5_CH1/IFC_CS5_B/IFC_RB3_B */
#define MUX_SER2_3_SEL_GPIO     GPIO_BIT_OFFSET(26)
/* IFC_A27/GPIO2_27/ FTM5_EXTCLK/IFC_CS6_B */
#define MUX_SER5_6_SEL_GPIO     GPIO_BIT_OFFSET(27)
/*
 * GPIO bank 4
 */
/* IIC2_SDA/GPIO4_3/ SDHC_WP/FTM3_QD_PHB */
#define N_SDHC_CD_B_GPIO        GPIO_BIT_OFFSET(3)

/*
 * TCA6424 I2C GPIO expander on carrier
 */
#define CAR_I2C_IO_ADDR         0x22
/*
 * PORT 2
 */
#define TX_DISABLE_GPIO         0x4

/*
 * TCA6424 I2C GPIO expander on module
 * Possible TODO: move to other module related header file
 */
#define MOD_I2C_IO_ADDR         0x23
/*
 * PORT 1
 */
/* Reset for CS4315 XFI retimer (IFC_CS_N4) */
#define EDC_RST_N_3V3_GPIO      0x08
/* Clock enable PCIe slots (IFC_CS_N7) */
#define SLOT4_REFCLK_OE_N_GPIO  0x40
/*
 * PORT 2
 */
/* SGMII0 LINKSTAT (N_DMA1_DREQ0_B) */
#define SGMII0_LINKSTAT_GPIO    0x1
/* SGMII0 ACT (N_DMA1_DACK0_B) */
#define SGMII0_ACT_GPIO         0x2
/* SGMII1 LINKSTAT (N_DMA1_DDONE_B) */
#define SGMII1_LINKSTAT_GPIO    0x4
/* SGMII1 ACT (N_DMA2_DREQ0_B) */
#define SGMII1_ACT_GPIO         0x8
/* Reset for RGMII PHY 1 (N_DMA2_DDONE_B) */
#define ETH2_RESET_N_3V3_GPIO   0x20
#define DEBUG_LED_GREEN_GPIO    0x40
#define DEBUG_LED_RED_GPIO      0x80

int scalys_carrier_init_pcie(void);
int scalys_carrier_setup_muxing(void);

#endif /* _DRAGON_FRUIT_H */
