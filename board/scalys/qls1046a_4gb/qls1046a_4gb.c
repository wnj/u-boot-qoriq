/*
 * Copyright 2019 Teledyne e2v
 * Copyright 2020 Scalys
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <i2c.h>
#include <fdt_support.h>
#include <asm/io.h>
#include <asm/arch/clock.h>
#include <asm/arch/fsl_serdes.h>
#include <asm/arch/ppa.h>
#include <asm/arch/soc.h>
#include <asm/arch-fsl-layerscape/fsl_icid.h>
#include <hwconfig.h>
#include <ahci.h>
#include <mmc.h>
#include <scsi.h>
#include <fm_eth.h>
#include <fsl_csu.h>
#include <fsl_esdhc.h>
#include <fsl_sec.h>
#include <fsl_ifc.h>

#include "dragonfruit.h"
#include <board_configuration_data.h>

DECLARE_GLOBAL_DATA_PTR;

struct ifc_regs ifc_cfg_norcs0_nandcs1[CONFIG_SYS_FSL_IFC_BANK_COUNT] = {
	{
		"nor0",
		CONFIG_SYS_NOR_CSPR,
		CONFIG_SYS_NOR_CSPR_EXT,
		CONFIG_SYS_NOR_AMASK,
		CONFIG_SYS_NOR_CSOR,
		{
			CONFIG_SYS_NOR_FTIM0,
			CONFIG_SYS_NOR_FTIM1,
			CONFIG_SYS_NOR_FTIM2,
			CONFIG_SYS_NOR_FTIM3
		},

	},
	{
		"nand",
		CONFIG_SYS_NAND_CSPR,
		CONFIG_SYS_NAND_CSPR_EXT,
		CONFIG_SYS_NAND_AMASK,
		CONFIG_SYS_NAND_CSOR,
		{
			CONFIG_SYS_NAND_FTIM0,
			CONFIG_SYS_NAND_FTIM1,
			CONFIG_SYS_NAND_FTIM2,
			CONFIG_SYS_NAND_FTIM3
		},
	}
};

struct ifc_regs ifc_cfg_nandcs0_norcs1[CONFIG_SYS_FSL_IFC_BANK_COUNT] = {
	{
		"nand",
		CONFIG_SYS_NAND_CSPR,
		CONFIG_SYS_NAND_CSPR_EXT,
		CONFIG_SYS_NAND_AMASK,
		CONFIG_SYS_NAND_CSOR,
		{
			CONFIG_SYS_NAND_FTIM0,
			CONFIG_SYS_NAND_FTIM1,
			CONFIG_SYS_NAND_FTIM2,
			CONFIG_SYS_NAND_FTIM3
		},
	},
	{
		"nor0",
		CONFIG_SYS_NOR_CSPR,
		CONFIG_SYS_NOR_CSPR_EXT,
		CONFIG_SYS_NOR_AMASK,
		CONFIG_SYS_NOR_CSOR,
		{
			CONFIG_SYS_NOR_FTIM0,
			CONFIG_SYS_NOR_FTIM1,
			CONFIG_SYS_NOR_FTIM2,
			CONFIG_SYS_NOR_FTIM3
		},
	}
};

void ifc_cfg_boot_info(struct ifc_regs_info *regs_info)
{
#if defined(CONFIG_NAND_BOOT)
	regs_info->regs = ifc_cfg_nandcs0_norcs1;
#elif defined(CONFIG_NOR_BOOT) || defined(CONFIG_SD_BOOT)
	regs_info->regs = ifc_cfg_norcs0_nandcs1;
#else
#error "Unkown or unsupported boot source"
#endif
    	regs_info->cs_size = CONFIG_SYS_FSL_IFC_BANK_COUNT;
	
#if defined(DEBUG) || 1
	print_ifc_regs();
#endif
}

int board_early_init_f(void)
{
	fsl_lsch2_early_init_f();

	return 0;
}

int board_mmc_getcd(struct mmc *mmc)
{
    return CONFIG_ESDHC_DETECT_QUIRK;
}

#ifndef CONFIG_SPL_BUILD

int checkboard(void)
{	
	puts("Board: Teledyne QLS1046A 4GB (DDR4) on Scalys daughter board.\nBoot from: ");

#ifdef CONFIG_SD_BOOT
	puts("SD\n");
#elif defined(CONFIG_NOR_BOOT)
	puts("NOR (IFC)\n");
#elif defined(CONFIG_NAND_BOOT)
	puts("NAND (IFC)\n");
#else
	puts("Unknown/unsupported source.\n");
#endif
	return 0;
}

int board_init(void)
{
	scalys_carrier_setup_muxing();
    
#ifdef CONFIG_PCI
	scalys_carrier_init_pcie();
#endif
	
#ifdef CONFIG_SECURE_BOOT
#error SECURE_BOOT not supported
#endif

#ifdef CONFIG_FSL_CAAM
	sec_init();
#endif

#ifdef CONFIG_FSL_LS_PPA
	ppa_init();
#endif

	return 0;
}

#ifdef CONFIG_MISC_INIT_R
int misc_init_r(void)
{
	int ret;
	struct ccsr_scfg *scfg = (struct ccsr_scfg *)CONFIG_SYS_FSL_SCFG_ADDR;
	u32 usb_pwrfault;
	uint8_t i2c_data;
	const void* bcd_dtc_blob;

	/* configure mux to IIC3_SCL/IIC3_SDA and IIC4_SCL/IIC4_SDA */
	out_be32(&scfg->rcwpmuxcr0, 0x0000);

	/* USB1 ctrl drives USB_DRVVBUS I/O */
	out_be32(&scfg->usbdrvvbus_selcr, SCFG_USBDRVVBUS_SELCR_USB1);

	/* USB1 & USB2 share the same USB_PWRFAULT/GPIO4_30 input pin */
	usb_pwrfault = (SCFG_USBPWRFAULT_INACTIVE <<
			SCFG_USBPWRFAULT_USB3_SHIFT) |
			(SCFG_USBPWRFAULT_SHARED <<
			SCFG_USBPWRFAULT_USB2_SHIFT) |
			(SCFG_USBPWRFAULT_SHARED <<
			SCFG_USBPWRFAULT_USB1_SHIFT);
	out_be32(&scfg->usbpwrfault_selcr, usb_pwrfault);
	
	/* 
	 * Initialize and set the LED's on the module to indicate u-boot is alive
	 * DEBUG_LED_GREEN : pin 6 of port 2 of i2c io expander
	 * DEBUG_LED_RED : pin 7 of port 2 of i2c io expander
	 */
	#define DEBUG_LED_GREEN		0x40
	#define DEBUG_LED_RED		0x80
	/* Set green led on and red led off */
	i2c_set_bus_num(0);
	ret = i2c_read(MOD_I2C_IO_ADDR, 0x0E, 1, &i2c_data, 1);
	i2c_data &= ~(DEBUG_LED_GREEN | DEBUG_LED_RED);
	ret = i2c_write(MOD_I2C_IO_ADDR, 0x0E, 1, &i2c_data, 1);
        ret = i2c_read(MOD_I2C_IO_ADDR, 0x06, 1, &i2c_data, 1);
	i2c_data |= DEBUG_LED_GREEN;
        i2c_data &= ~DEBUG_LED_RED;
        ret = i2c_write(MOD_I2C_IO_ADDR, 0x06, 1, &i2c_data, 1);
	
	/* Read BCD data from I2C eeprom */
	bcd_dtc_blob = get_boardinfo_eeprom();
	if (bcd_dtc_blob != NULL) {
		/* Board Configuration Data is intact, ready for parsing */
		ret = add_mac_addressess_to_env(bcd_dtc_blob);
		if (ret != 0) {
			printf("Error adding BCD data to environement\n");
		}
	}

	return 0;
}
#endif

int ft_board_setup(void *blob, bd_t *bd)
{
#ifdef CONFIG_NR_DRAM_BANKS
	u64 base[CONFIG_NR_DRAM_BANKS];
	u64 size[CONFIG_NR_DRAM_BANKS];

	/* fixup DT for the two DDR banks */
	base[0] = gd->bd->bi_dram[0].start;
	size[0] = gd->bd->bi_dram[0].size;
	base[1] = gd->bd->bi_dram[1].start;
	size[1] = gd->bd->bi_dram[1].size;

	fdt_fixup_memory_banks(blob, base, size, 2);
	ft_cpu_setup(blob, bd);
#endif

#ifdef CONFIG_SYS_DPAA_FMAN
	fdt_fixup_fman_ethernet(blob);
#endif

	fdt_fixup_icid(blob);

	return 0;
}

#endif /* !CONFIG_SPL_BUILD */

u8 flash_read8(void *addr)
{
	return __raw_readb(addr + 1);
}

void flash_write16(u16 val, void *addr)
{
	u16 shftval = (((val >> 8) & 0xff) | ((val << 8) & 0xff00));

	__raw_writew(shftval, addr);
}

u16 flash_read16(void *addr)
{
	u16 val = __raw_readw(addr);

	return (((val) >> 8) & 0x00ff) | (((val) << 8) & 0xff00);
}

