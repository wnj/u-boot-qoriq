#
# Copyright 2020 Scalys
#
# SPDX-License-Identifier:	GPL-2.0+
#

UBOOTINCLUDE += -I$(srctree)/board/$(VENDOR)/common/

obj-y	+= ddr.o
obj-y	+= qls1046a_4gb.o
ifndef CONFIG_SPL_BUILD
obj-y	+= eth.o
obj-y 	+= dragonfruit.o
endif

