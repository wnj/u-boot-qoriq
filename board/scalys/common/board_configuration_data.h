#ifndef _BCD_H
#define _BCD_H

#define BCD_LENGTH_SIZE	4
#define BCD_HASH_SIZE	4

#ifndef BCD_I2C_BUS
#define BCD_I2C_BUS 0
#endif

#ifndef BCD_I2C_ADDRESS
#define BCD_I2C_ADDRESS 0x51
#endif

#ifndef BCD_EEPROM_SIZE
#define BCD_EEPROM_SIZE 0x10000
#endif

const void* get_boardinfo_eeprom(void);
int add_mac_addressess_to_env(const void* blob);

#endif /* _BCD_H */
