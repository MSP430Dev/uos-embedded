typedef struct _flash_t {
	unsigned short	*memory;
	unsigned short	flash_id;
	unsigned short	mfr_id;
	unsigned short	*flash_addr_555;
	unsigned short	*flash_addr_2aa;
	unsigned short	flash_cmd_aa;
	unsigned short  flash_cmd_55;
	unsigned short  flash_cmd_90;
	unsigned short	flash_cmd_80;
	unsigned short	flash_cmd_10;
	unsigned short	flash_cmd_a0;
	unsigned short	flash_cmd_30;
	unsigned short	flash_cmd_f0;
	unsigned short  id_alliance;
	unsigned short  id_amd;
	unsigned short  id_29lv800_b;
	unsigned short  id_29lv800_t;
} flash_t;

int flash_open (flash_t *f, unsigned long base);
void flash_get_name (flash_t *f, char *name);
void flash_erase_chip (flash_t *f);
void flash_erase_sector (flash_t *f, unsigned long addr);
int flash_is_erased (flash_t *f, unsigned long addr);
void flash_write16 (flash_t *f, unsigned long addr, unsigned short val);
