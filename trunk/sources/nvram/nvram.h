/*
 * Simple interface to on-chip persistent memory.
 */
#ifndef _NVRAM_H_
#define _NVRAM_H_ 1

struct _timer_t;

void nvram_init (void);
void nvram_protect (struct _timer_t *timer);
void nvram_unprotect (struct _timer_t *timer);

void nvram_write_byte (unsigned addr, unsigned char c);
unsigned char nvram_read_byte (unsigned addr);

void nvram_write16 (unsigned addr, uint16_t val);
uint16_t nvram_read16 (unsigned addr);

void nvram_write32 (unsigned addr, uint32_t val);
uint32_t nvram_read32 (unsigned addr);

#endif /* _NVRAM_H_ */
