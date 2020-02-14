/*
 * Copyright 2016 Freescale Semiconductor, Inc.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <fsl_ddr.h>
#include <fsl_ddr_sdram.h>
#include <fsl_ddr_dimm_params.h>
#include "ddr.h"
#ifdef CONFIG_FSL_DEEP_SLEEP
#include <fsl_sleep.h>
#endif
#include <asm/arch/clock.h>

DECLARE_GLOBAL_DATA_PTR;

dimm_params_t ddr_raw_timing = {
	.n_ranks		= 1,
	.rank_density		= 0x100000000ULL,
	.capacity		= 0x100000000ULL,
	.device_width		= 64,
	.primary_sdram_width 	= 64,
	.ec_sdram_width 	= 8,
	.registered_dimm 	= 0,
	.mirrored_dimm 		= 0,
	.n_row_addr 		= 16,
	.n_col_addr 		= 10,
	.bank_addr_bits		= 0,
	.bank_group_bits	= 1,
	.edc_config 		= EDC_ECC,

#ifdef DDR_1600MTS
	.burst_lengths_bitmask 	= 0x0c,
	.tckmin_x_ps 		= 1250,
	.tckmax_ps		= 1499,
	.caslat_x		= 0x1400,
	.taa_ps 		= 12500,
	.trcd_ps 		= 12500,
	.trp_ps			= 12500,
	.tras_ps		= 32000,
	.trfc1_ps		= 260000,
	.trrds_ps		= 5000,
	.trrdl_ps		= 5000,
	.tccdl_ps		= 5000,
	.refresh_rate_ps 	= 3900000,
	.tfaw_ps 		= 21000,
#else   /* up to 2400 */
	.burst_lengths_bitmask 	= 0x0c,
	.tckmin_x_ps 		=  935,
	.tckmax_ps		= 1499,
	.caslat_x		= 0xFF400,
	.taa_ps 		= 13750,
	.trcd_ps 		= 13750,
	.trp_ps			= 13750,
	.tras_ps		= 32000,
	.trfc1_ps		= 260000,
	.trrds_ps		= 5000,
	.trrdl_ps		= 5000,
	.tccdl_ps		= 5000,
	.refresh_rate_ps 	= 3900000,
	.tfaw_ps 		= 21000,
#endif
	
	/* Mapping updated according to mapping in ctrl_regs.c */
	/* dq_map_0 */
	.dq_mapping[ 0] = 0x11,
	.dq_mapping[ 1] = 0x35,
	.dq_mapping[ 2] = 0x25,
	.dq_mapping[ 3] = 0x13,
	.dq_mapping[ 4] = 0x29,
	/* dq_map_1 */
	.dq_mapping[ 5] = 0x0B,
	.dq_mapping[ 6] = 0x12,
	.dq_mapping[ 7] = 0x31,
	.dq_mapping[10] = 0x15,
	.dq_mapping[11] = 0x2F,
	/* dq_map_2 */
	.dq_mapping[12] = 0x03,
	.dq_mapping[13] = 0x32,
	.dq_mapping[14] = 0x2F,
	.dq_mapping[15] = 0x14,
	.dq_mapping[16] = 0x18,
	/* dq_map_3 */
	.dq_mapping[17] = 0x24,
	.dq_mapping[ 8] = 0x0B,
	.dq_mapping[ 9] = 0x2A,
	.dq_mapping_ors = 0,
    
};

void fsl_ddr_board_options(memctl_options_t *popts,
			   dimm_params_t *pdimm,
			   unsigned int ctrl_num)
{
	int i;
	
	if (ctrl_num != 0) {
		printf("Only 1 memory controller supported, but %i requested\n",
		       ctrl_num);
		return; 
	}
	
	if (pdimm == NULL ) {
		printf("Error, no valid dimm pararmeter supplied\n");
		return;
	}

	if (!pdimm->n_ranks) {
		printf("No ranks in dimm parameters. Configuration error?\n");
		return;
	}

	/* set odt_rd_cfg and odt_wr_cfg. */
	for (i = 0; i < CONFIG_CHIP_SELECTS_PER_CTRL; i++) {
		popts->cs_local_opts[i].odt_wr_cfg = 4;
	}

	for (i = 0; i < CONFIG_CHIP_SELECTS_PER_CTRL; i++) {
		popts->cs_local_opts[i].odt_rd_cfg = 4;
	}
	
	popts->wrlvl_override = 1;
	popts->wrlvl_en       = 1;
	popts->wrlvl_sample   = 0xf;
#ifdef DDR_1600MTS
	popts->wrlvl_start    = 5;
	popts->wrlvl_ctl_2    = 0x05060605;
	popts->wrlvl_ctl_3    = 0x06050505;
#else
	popts->wrlvl_start    = 6;
	popts->wrlvl_ctl_2    = 0x06060606;
	popts->wrlvl_ctl_3    = 0x06050505;
#endif

#ifndef CONFIG_SYS_FSL_DDR4
#error CONFIG_SYS_FSL_DDR4 not defined
#endif

#ifdef DDR_1600MTS
#warning ----------- 1600 settings working - but to be fully validated with DDRV ! ---------------
	/* half strength */
	popts->half_strength_driver_enable = 1;

	popts->ddr_cdr1 = DDR_CDR1_DHC_EN | DDR_CDR1_ODT(DDR_CDR_ODT_40ohm);
	popts->ddr_cdr2 = DDR_CDR2_ODT(DDR_CDR_ODT_40ohm) |
	DDR_CDR2_VREF_OVRD(70);   /* Vref = 70% */
#else
	/* half strength */
	popts->half_strength_driver_enable = 0;

	popts->ddr_cdr1 = DDR_CDR1_DHC_EN | DDR_CDR1_ODT(DDR_CDR_ODT_60ohm);
	popts->ddr_cdr2 = DDR_CDR2_ODT(DDR_CDR_ODT_60ohm) |
	DDR_CDR2_VREF_OVRD(70);   /* Vref = 70% */
#endif

	/* rtt and rtt_wr override */
	popts->rtt_override = 0; /* */
	
	/* Enable ZQ calibration */
	popts->zq_en = 1; /* */

	/* Clock is launched xxx applied cycle after address/command */
	popts->clk_adjust = 8; 
	
	/* Optimized cpo */
#ifdef DDR_1600MTS
	popts->cpo_sample = 0x41; /* 1600MTs */
#else
	popts->cpo_sample = 0x47; /* 2100MTs */
#endif
}

int fsl_ddr_get_dimm_params(dimm_params_t *pdimm,
			    unsigned int controller_number,
			    unsigned int dimm_number)
{
	const char dimm_model[] = "Soldered-down DDR4";
	
	if (((controller_number == 0) && (dimm_number == 0)) ||
		((controller_number == 1) && (dimm_number == 0))) {
		memcpy(pdimm, &ddr_raw_timing, sizeof(dimm_params_t));
		memset(pdimm->mpart, 0, sizeof(pdimm->mpart));
		memcpy(pdimm->mpart, dimm_model, sizeof(pdimm->mpart) - 1);
	}
	
	return 0;
}

int fsl_initdram(void)
{
	phys_size_t dram_size;

#if defined(CONFIG_SPL) && !defined(CONFIG_SPL_BUILD)
	gd->ram_size = fsl_ddr_sdram_size();

	return 0;
#else
#ifdef DDR_1600MTS
	debug("Initializing DDR....using 1600MTS settings\n");
#else
	debug("Initializing DDR....using 2100MTS settings\n");
#endif
	dram_size = fsl_ddr_sdram();

	erratum_a009942_check_cpo();
#endif

	erratum_a008850_post();

	gd->ram_size = dram_size;

	return 0;
}
