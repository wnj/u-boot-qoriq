/*
 * Copyright 2018 NXP
 * SPDX-License-Identifier:	GPL-2.0+        BSD-3-Clause
 *
 * Author York Sun <york.sun@nxp.com>
 */

#include <common.h>
#include "include/dimm.h"
#include "include/pie.h"
#include "include/io.h"
#include "include/init.h"
#include "include/dimm.h"

static void load_pieimage(const unsigned int ctrl_num,
			  enum dimm_types dimm_type)
{
	int i;
	int size;
	const struct pie *image = NULL;

	switch (dimm_type) {
	case UDIMM:
	case SODIMM:
	case NODIMM:
		image = image_udimm;
		size = ARRAY_SIZE(image_udimm);
		break;
	case RDIMM:
		image = image_rdimm;
		size = ARRAY_SIZE(image_rdimm);
		break;
	case LRDIMM:
		image = image_lrdimm;
		size = ARRAY_SIZE(image_lrdimm);
		break;
	default:
		printf("Unsupported DIMM type\n");
		break;
	}

	if (image) {
		for (i = 0; i < size; i++)
			phy_io_write16(ctrl_num, image[i].addr, image[i].data);
	}
}

static void prog_acsm_playback(const unsigned int ctrl_num,
			       const struct input *input, const void *msg)
{
	int vec;
	const struct ddr4r1d *msg_blk;
	uint16_t acsmplayback[2][3];
	uint32_t f0rc0a;
	uint32_t f0rc3x;
	uint32_t f0rc5x;

	if (input->basic.dimm_type != RDIMM)
		return;

	msg_blk = msg;
	f0rc0a = (msg_blk->f0rc0a_d0 & 0xf) | 0xa0;
	f0rc3x = (msg_blk->f0rc3x_d0 & 0xff) | 0x300;
	f0rc5x = (input->adv.phy_gen2_umctl_f0rc5x & 0xff) | 0x500;

	acsmplayback[0][0] = 0x3ff & f0rc0a;
	acsmplayback[1][0] = (0x1c00 & f0rc0a) >> 10;
	acsmplayback[0][1] = 0x3ff & f0rc3x;
	acsmplayback[1][1] = (0x1c00 & f0rc3x) >> 10;
	acsmplayback[0][2] = 0x3ff & f0rc5x;
	acsmplayback[1][2] = (0x1c00 & f0rc5x) >> 10;
	for (vec = 0; vec < 3; vec++) {
		phy_io_write16(ctrl_num, t_acsm | (csr_acsm_playback0x0_addr +
			       (vec << 1)), acsmplayback[0][vec]);
		phy_io_write16(ctrl_num, t_acsm | (csr_acsm_playback1x0_addr +
			       (vec << 1)), acsmplayback[1][vec]);
	}
}

static void prog_acsm_ctr(const unsigned int ctrl_num,
			  const struct input *input)
{
	if (input->basic.dimm_type != RDIMM)
		return;

	phy_io_write16(ctrl_num, t_acsm | csr_acsm_ctrl13_addr, 0xf <<
				csr_acsm_cke_enb_lsb);
	phy_io_write16(ctrl_num, t_acsm | csr_acsm_ctrl0_addr,
		       csr_acsm_par_mode_mask);
}

/* Only RDIMM requires msg_blk */
void i_load_pie(const unsigned int ctrl_num, const struct input *input,
		const void *msg)
{
	phy_io_write16(ctrl_num, t_apbonly | csr_micro_cont_mux_sel_addr, 0);

	load_pieimage(ctrl_num, input->basic.dimm_type);

	phy_io_write16(ctrl_num, t_initeng | csr_seq0bdisable_flag0_addr,
		       0x0000);
	phy_io_write16(ctrl_num, t_initeng | csr_seq0bdisable_flag1_addr,
		       0x0173);
	phy_io_write16(ctrl_num, t_initeng | csr_seq0bdisable_flag2_addr,
		       0x0060);
	phy_io_write16(ctrl_num, t_initeng | csr_seq0bdisable_flag3_addr,
		       0x6110);
	phy_io_write16(ctrl_num, t_initeng | csr_seq0bdisable_flag4_addr,
		       0x2152);
	phy_io_write16(ctrl_num, t_initeng | csr_seq0bdisable_flag5_addr,
		       0xdfbd);
	phy_io_write16(ctrl_num, t_initeng | csr_seq0bdisable_flag6_addr,
		       input->basic.dimm_type == RDIMM &&
			input->adv.phy_gen2_umctl_opt == 1 ?
		       0x6000 : 0xffff);
	phy_io_write16(ctrl_num, t_initeng | csr_seq0bdisable_flag7_addr,
		       0x6152);
	prog_acsm_playback(ctrl_num, input, msg);		/* rdimm */
	prog_acsm_ctr(ctrl_num, input);				/* rdimm */
	phy_io_write16(ctrl_num, t_drtub | csr_ucclk_hclk_enables_addr,
		       input->basic.dimm_type == RDIMM ? 0x2 : 0);

	phy_io_write16(ctrl_num, t_apbonly | csr_micro_cont_mux_sel_addr, 1);
}