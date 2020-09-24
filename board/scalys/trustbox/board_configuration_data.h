/*
 * Copyright 2019 Scalys B.V.
 * opensource@scalys.com
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef _BCD_H
#define _BCD_H

#define BCD_LENGTH_SIZE	4

#define BCD_FLASH_SIZE 0x100000 /* 1 MBytes */

#define BCD_HASH_SIZE	4

enum trustbox_flash_types {
	PRIMARY_FLASH_DEVICE,
	RESCUE_FLASH_DEVICE,
};

const void* get_boardinfo_rescue_flash(void);
struct udevice* select_qspi_flash_device(enum trustbox_flash_types);
const char* get_qspi_flash_name(void);
int add_mac_addressess_to_env(const void* blob);

#endif /* _BCD_H */
