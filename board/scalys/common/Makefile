#
# (C) Copyright 2006
# Wolfgang Denk, DENX Software Engineering, wd@denx.de.
#
# SPDX-License-Identifier:	GPL-2.0+
#

obj-y	+= board_configuration_data.o

MINIMAL=

ifdef CONFIG_SPL_BUILD
ifdef CONFIG_SPL_INIT_MINIMAL
MINIMAL=y
endif
endif

ifdef MINIMAL
# necessary to create built-in.o
obj- := __dummy__.o
else
obj-$(CONFIG_FSL_CADMUS)	+= ../../freescale/common/cadmus.o
obj-$(CONFIG_FSL_VIA)		+= ../../freescale/common/cds_via.o
obj-$(CONFIG_FMAN_ENET)	+= ../../freescale/common/fman.o
obj-$(CONFIG_FSL_PIXIS)	+= ../../freescale/common/pixis.o
ifndef CONFIG_SPL_BUILD
obj-$(CONFIG_FSL_NGPIXIS)	+= ../../freescale/common/ngpixis.o
obj-$(CONFIG_VID)		+= ../../freescale/common/vid.o
endif
obj-$(CONFIG_FSL_QIXIS)	+= ../../freescale/common/qixis.o
obj-$(CONFIG_PQ_MDS_PIB)	+= ../../freescale/common/pq-mds-pib.o
ifndef CONFIG_SPL_BUILD
obj-$(CONFIG_ID_EEPROM)	+= ../../freescale/common/sys_eeprom.o
endif
obj-$(CONFIG_FSL_SGMII_RISER)	+= ../../freescale/common/sgmii_riser.o
ifndef CONFIG_RAMBOOT_PBL
obj-$(CONFIG_FSL_FIXED_MMC_LOCATION)	+= ../../freescale/common/sdhc_boot.o
endif

obj-$(CONFIG_FSL_DIU_CH7301)	+= ../../freescale/common/diu_ch7301.o

ifdef CONFIG_ARM
obj-$(CONFIG_DEEP_SLEEP)		+= ../../freescale/common/arm_sleep.o
else
obj-$(CONFIG_DEEP_SLEEP)		+= ../../freescale/common/mpc85xx_sleep.o
endif

obj-$(CONFIG_FSL_DCU_SII9022A)    += ../../freescale/common/dcu_sii9022a.o

obj-$(CONFIG_TARGET_MPC8541CDS)	+= ../../freescale/common/cds_pci_ft.o
obj-$(CONFIG_TARGET_MPC8548CDS)	+= ../../freescale/common/cds_pci_ft.o
obj-$(CONFIG_TARGET_MPC8555CDS)	+= ../../freescale/common/cds_pci_ft.o

obj-$(CONFIG_TARGET_MPC8536DS)	+= ../../freescale/common/ics307_clk.o
obj-$(CONFIG_TARGET_MPC8572DS)	+= ../../freescale/common/ics307_clk.o
obj-$(CONFIG_TARGET_P1022DS)		+= ../../freescale/common/ics307_clk.o
obj-$(CONFIG_P2020DS)		+= ../../freescale/common/ics307_clk.o
obj-$(CONFIG_TARGET_P3041DS)		+= ../../freescale/common/ics307_clk.o
obj-$(CONFIG_TARGET_P4080DS)		+= ../../freescale/common/ics307_clk.o
obj-$(CONFIG_TARGET_P5020DS)		+= ../../freescale/common/ics307_clk.o
obj-$(CONFIG_TARGET_P5040DS)		+= ../../freescale/common/ics307_clk.o
obj-$(CONFIG_VSC_CROSSBAR)    += ../../freescale/common/vsc3316_3308.o
obj-$(CONFIG_IDT8T49N222A)	+= ../../freescale/common/idt8t49n222a_serdes_clk.o
obj-$(CONFIG_ZM7300)		+= ../../freescale/common/zm7300.o
obj-$(CONFIG_POWER_PFUZE100)	+= ../../freescale/common/pfuze.o
obj-$(CONFIG_POWER_MC34VR500)	+= ../../freescale/common/mc34vr500.o

obj-$(CONFIG_LS102XA_STREAM_ID)	+= ../../freescale/common/ls102xa_stream_id.o

# deal with common files for P-series corenet based devices
obj-$(CONFIG_TARGET_P2041RDB)	+= ../../freescale/common/p_corenet/
obj-$(CONFIG_TARGET_P3041DS)	+= ../../freescale/common/p_corenet/
obj-$(CONFIG_TARGET_P4080DS)	+= ../../freescale/common/p_corenet/
obj-$(CONFIG_TARGET_P5020DS)	+= ../../freescale/common/p_corenet/
obj-$(CONFIG_TARGET_P5040DS)	+= ../../freescale/common/p_corenet/

obj-$(CONFIG_LAYERSCAPE_NS_ACCESS)	+= ../../freescale/common/ns_access.o

ifdef CONFIG_SECURE_BOOT
obj-$(CONFIG_CMD_ESBC_VALIDATE) += ../../freescale/common/fsl_validate.o cmd_esbc_validate.o
endif
obj-$(CONFIG_CHAIN_OF_TRUST) += ../../freescale/common/fsl_chain_of_trust.o

endif
