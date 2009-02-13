/*
 * Simple interface to on-chip persistent memory.
 */
#ifndef _EEPROM_H_
#define _EEPROM_H_ 1

void eeprom_init (void);
void eeprom_write_byte (unsigned addr, unsigned char c);
unsigned char eeprom_read_byte (unsigned addr);

#endif /* _EEPROM_H_ */
