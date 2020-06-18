#
# Copyright 2019 Scalys B.V.
# opensource@scalys.com
#
# SPDX-License-Identifier:      GPL-2.0+
#

obj-y += trustbox.o
obj-y += usb_trustbox.o
obj-y += board_configuration_data.o
obj-$(CONFIG_FSL_PFE) += eth.o

obj-$(CONFIG_CHAIN_OF_TRUST) += ../../freescale/common/fsl_chain_of_trust.o
obj-$(CONFIG_SECURE_BOOT) += ../../freescale/common/fsl_validate.o ../../freescale/common/cmd_esbc_validate.o

obj-$(CONFIG_LAYERSCAPE_NS_ACCESS)	+= ../../freescale/common/ns_access.o
