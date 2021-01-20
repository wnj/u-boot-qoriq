# Copyright 2021 Scalys B.V.
# opensource@scalys.com
#
# SPDX-License-Identifier:	GPL-2.0+

obj-y += ddr.o
obj-y += trustbox-xl.o
ifndef CONFIG_SPL_BUILD
obj-$(CONFIG_NET) += eth.o
endif

obj-$(CONFIG_CHAIN_OF_TRUST) += ../../freescale/common/fsl_chain_of_trust.o
obj-$(CONFIG_SECURE_BOOT) += ../../freescale/common/fsl_validate.o ../../freescale/common/cmd_esbc_validate.o

obj-$(CONFIG_LAYERSCAPE_NS_ACCESS) += ../../freescale/common/ns_access.o
obj-$(CONFIG_POWER_MC34VR500) += ../../freescale/common/mc34vr500.o
obj-$(CONFIG_ID_EEPROM) += ../../freescale/common/sys_eeprom.o
