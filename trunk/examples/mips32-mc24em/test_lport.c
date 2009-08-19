/*
 * Testing LPORT on Elvees MC-24EM board.
 *
 * Authors: Kirill Salyamov (ksalyamov@elvees.com), Evgeny Guskov (eguskov@elvees.com)
 * Copyright (C) 2009 ELVEES
 *
 * Created: 05.08.2009
 * Last modify: 19.08.2009
 */

#include <runtime/lib.h>
#include <kernel/uos.h>
#include <mem/mem.h>

#define STACK_SIZE      1000

#define CRAM_START	0xB8000000
#define CRAM_SIZE	(32*1024)
#define SRAM_START	0xBFC00000
#define SRAM_SIZE	(1024*1024)
#define SDRAM_START	0xA0000000

#define BLOCK_SIZE_DEFAULT 16
#define BLOCK_SIZE_MAX     2048

#define SDRAM_MEM_START	SDRAM_START
#define SDRAM_MEM_SIZE	16*1024
#define SDRAM_MEM_END   (SDRAM_START + SDRAM_MEM_SIZE)

#define CRAM_MEM_START	(uint32_t)(&_end)
#define CRAM_MEM_SIZE	16*1024
#define CRAM_MEM_END    (CRAM_START + CRAM_MEM_SIZE)

#define SRAM_MEM_START	(uint32_t)(&_etext)
#define SRAM_MEM_SIZE	16*1024
#define SRAM_MEM_END    (SRAM_START + SRAM_MEM_SIZE)

#define SDRAM_SIZE	(128*1024*1024)

#define CRAM 		'1'
#define SRAM 		'2'
#define SDRAM 		'3'

#define MC_LCSR_LEN      	0
#define MC_LCSR_LTRAN    	1
#define MC_LCSR_LCLK_RATE	2
#define MC_LCSR_LDW     	6

#define CYCLE_RATE		50
#define CYCLE_RATE_COMPLEX	5
#define TEST_COUNT		6

/**\~english
 * Data structure for holding a DMA self-initialization chain.
 *
 * \~russian
 * Структура для хранения цепочки самоинициализации DMA.
 */
typedef struct _dma_chain_t {
	uint32_t ir;
	uint32_t or;
	uint32_t y;
	uint32_t cp;
	uint32_t csr;
} dma_chain_t;

/**\~english
 * Data structure for holding a LPORT configuration.
 *
 * \~russian
 * Структура для хранения конфигурации LPORT.
 */
typedef struct {
	uint32_t pair; 		/* Which pair of lports; 0 - LP0,LP1; 1 - LP2,LP3; */
	uint32_t dir; 		/* Direction */
	uint32_t dma; 		/* Usage dma */
	uint32_t clk; 		/* Transfer speed (CLK/4  CLK/8) */
	uint32_t size; 		/* Transfer size (4 or 8) */
	uint32_t verify;
} lport_config_t;

void menu (void);
void main_console (void *data);

/* Menu for start test LP0,LP1 with DMA */
void menu_dma01 (void);
/* Menu for start test LP2,LP3 with DMA */
void menu_dma23 (void);

/* Start one of LPORT's tests for the pair of memory without DMA */
uint32_t check_lport01_ram (char from, char to, uint32_t check);
uint32_t check_lport23_ram (char from, char to, uint32_t check);
/* Start one of LPORT's tests for the pair of memory with DMA */
uint32_t check_lport01_dma (char from, char to, uint32_t check);
uint32_t check_lport23_dma (char from, char to, uint32_t check);

/* LPORT's tests for the pair of memory without DMA */
void test_lport01_ram (void);
void test_lport23_ram (void);
/* LPORT's tests for the pair of memory with DMA */
void test_lport01_dma (uint32_t **src, uint32_t **dst, char from, char to, uint32_t test_num);
void test_lport23_dma (uint32_t **src, uint32_t **dst, char from, char to, uint32_t test_num);

/* Test all */
void test_complex (void);

void setup_dma_chain (dma_chain_t *chain, void *addr, uint32_t word_count, dma_chain_t *chain_next);

void prepare_test_array (uint32_t *array, size_t size, uint32_t test_num);

dma_chain_t lport0_chain;
dma_chain_t lport1_chain;
dma_chain_t lport2_chain;
dma_chain_t lport3_chain;

lport_config_t lport_config;

uint32_t *in_array_cram  = 0;
uint32_t *out_array_cram = 0;

uint32_t *in_array_sram  = 0;
uint32_t *out_array_sram = 0;

uint32_t *in_array_sdram  = 0;
uint32_t *out_array_sdram = 0;

uint32_t block_size = BLOCK_SIZE_DEFAULT;

mem_pool_t pool_sdram;
mem_pool_t pool_cram;
mem_pool_t pool_sram;

ARRAY (stack_console, STACK_SIZE);
extern void _etext ();
extern void _end ();

/**\~english
 * Fill array test data 
 *
 * \~russian
 * Заполнение массива тестовыми данными
 */
void
prepare_test_array (uint32_t *array, size_t size, uint32_t test_num)
{
	uint32_t idx;
	for (idx = 0; idx < size; idx++) {
		if (test_num == 0) {
			array[idx] = 0x55555555;
		}

		if (test_num == 1) {
			array[idx] = 0xAAAAAAAA;
		}
		
		if ((test_num == 2)&&(idx%2 == 0)) {
			array[idx] = 0x55555555;
		}

		if ((test_num == 2)&&(idx%2 == 1)) {
			array[idx] = 0xAAAAAAAA;
		}

		if (test_num == 3) {
			array[idx] = 1 << (idx%32);
		}

		if (test_num == 4) {
			array[idx] = ~(1 << (idx%32));
		}

		if (test_num == 5) {
			array[idx] = idx;
		}
	}
}

/**\~english
 * Setup one node of DMA self-initialization chain
 *
 * \~russian
 * Установка одного звена цепочки самоинициализации DMA
 */
void 
setup_dma_chain (dma_chain_t *chain, void *addr, uint32_t word_count, dma_chain_t *chain_next)
{
	(*chain).ir = (uint32_t)(addr) & (0x1FFFFFFC);
	(*chain).or = 1;
	(*chain).y  = 0;
	(*chain).cp = (uint32_t)(chain_next) & (0x1FFFFFFC);

	if (chain_next != 0)
		(*chain).csr = (word_count << 16) | 0x1001;	
	else
		(*chain).csr = (word_count << 16) | 1;
}

/**\~english
 * Setup DMA chain and run DMA
 *
 * \~russian
 * Установка цепочки DMA и запуск DMA
 */
void 
test_lport01_dma (uint32_t **src, uint32_t **dst, char from, char to, uint32_t test_num)
{
	uint32_t *target_memory = 0;
	uint32_t *source_memory = 0;

	if (from == CRAM)
		source_memory = in_array_cram;
	if (from == SRAM)
		source_memory = in_array_sram;
	if (from == SDRAM)
		source_memory = in_array_sdram;

	if (to == CRAM)
		target_memory = out_array_cram;
	if (to == SRAM)
		target_memory = out_array_sram;
	if (to == SDRAM)
		target_memory = out_array_sdram;

	prepare_test_array (source_memory, block_size, test_num);

	setup_dma_chain (&lport0_chain, source_memory, block_size, 0);
	setup_dma_chain (&lport1_chain, target_memory, block_size, 0);

	(*dst) = target_memory;
	(*src) = source_memory;

	udelay (6);

	MC_CP_LPCH(1) = (((uint32_t)&lport1_chain) & (0x1FFFFFFC)) | 0x80000000;
	MC_CP_LPCH(0) = (((uint32_t)&lport0_chain) & (0x1FFFFFFC)) | 0x80000000;

	if (lport_config.dir == 0) {
		MC_LCSR (1) = (lport_config.dir << MC_LCSR_LTRAN) |
			(lport_config.clk << MC_LCSR_LCLK_RATE) | 
			(lport_config.size << MC_LCSR_LDW) | 
			(1 << MC_LCSR_LEN);

		MC_LCSR (0) = ((!lport_config.dir) << MC_LCSR_LTRAN) |
			(lport_config.clk << MC_LCSR_LCLK_RATE) |
			(lport_config.size << MC_LCSR_LDW) |
			(1 << MC_LCSR_LEN);
	}
	else {
		MC_LCSR (0) = (lport_config.dir << MC_LCSR_LTRAN) |
			(lport_config.clk << MC_LCSR_LCLK_RATE) |
			(lport_config.size << MC_LCSR_LDW) |
			(1 << MC_LCSR_LEN);

		MC_LCSR (1) = ((!lport_config.dir) << MC_LCSR_LTRAN) |
			(lport_config.clk << MC_LCSR_LCLK_RATE) |
			(lport_config.size << MC_LCSR_LDW) |
			(1 << MC_LCSR_LEN);
	}
}

/**\~english
 * Setup DMA chain and run DMA
 *
 * \~russian
 * Установка цепочки DMA и запуск DMA
 */
void
test_lport23_dma (uint32_t **src, uint32_t **dst, char from, char to, uint32_t test_num)
{
	uint32_t *target_memory = 0;
	uint32_t *source_memory = 0;

	if (from == CRAM)
		source_memory = in_array_cram;
	if (from == SRAM)
		source_memory = in_array_sram;
	if (from == SDRAM)
		source_memory = in_array_sdram;

	if (to == CRAM)
		target_memory = out_array_cram;
	if (to == SRAM)
		target_memory = out_array_sram;
	if (to == SDRAM)
		target_memory = out_array_sdram;

	prepare_test_array (source_memory, block_size, test_num);

	setup_dma_chain (&lport2_chain, source_memory, block_size, 0);
	setup_dma_chain (&lport3_chain, target_memory, block_size, 0);

	(*dst) = target_memory;
	(*src) = source_memory;

	udelay (6);

	MC_CP_LPCH(3) = (((uint32_t)&lport3_chain) & (0x1FFFFFFC)) | 0x80000000;
	MC_CP_LPCH(2) = (((uint32_t)&lport2_chain) & (0x1FFFFFFC)) | 0x80000000;

	if (lport_config.dir == 0) {
		MC_LCSR (3) = (lport_config.dir << MC_LCSR_LTRAN) |
			(lport_config.clk << MC_LCSR_LCLK_RATE) | 
			(lport_config.size << MC_LCSR_LDW) | 
			(1 << MC_LCSR_LEN);

		MC_LCSR (2) = ((!lport_config.dir) << MC_LCSR_LTRAN) |
			(lport_config.clk << MC_LCSR_LCLK_RATE) |
			(lport_config.size << MC_LCSR_LDW) |
			(1 << MC_LCSR_LEN);
	}
	else {
		MC_LCSR (2) = (lport_config.dir << MC_LCSR_LTRAN) |
			(lport_config.clk << MC_LCSR_LCLK_RATE) |
			(lport_config.size << MC_LCSR_LDW) |
			(1 << MC_LCSR_LEN);

		MC_LCSR (3) = ((!lport_config.dir) << MC_LCSR_LTRAN) |
			(lport_config.clk << MC_LCSR_LCLK_RATE) |
			(lport_config.size << MC_LCSR_LDW) |
			(1 << MC_LCSR_LEN);
	}
}

/**\~english
 * Select pair of memories and start test with DMA throught LPO-LP1
 *
 * \~russian
 * Выбор пары памятей и запуск теста с использованием DMA через LP0-LP1
 */
void
menu_dma01 (void)
{
	uint32_t check;
	char from;
	char to;
	char cmd;
	uint32_t error_count;
	uint32_t cycle_count = 0;

	debug_puts ("Select memories:\n\n");
try_again:
	debug_puts ("From: \n");
	debug_puts ("1. CRAM\n");
	debug_puts ("2. SRAM\n");
	debug_puts ("3. SDRAM\n");

	debug_puts ("Command: ");
	from = debug_getchar ();
	debug_printf ("%c\n", from);

	debug_puts ("\n\n To :\n");
	debug_puts ("1. CRAM\n");
	debug_puts ("2. SRAM\n");
	debug_puts ("3. SDRAM\n");

	debug_puts ("Command: ");
	to = debug_getchar ();
	debug_printf ("%c\n", to);

	if ((from > SDRAM) || (from < CRAM) || (to > SDRAM) || (to < CRAM)) {
		debug_puts ("Wrong input! Try again!\n\n");
		goto try_again;
	}

	for (;;) {
		for (check=0; check<TEST_COUNT; check++) {
			error_count = check_lport01_dma (from, to, check);
	
			if (lport_config.verify == 1)
				debug_printf ("Error count: %u\n", error_count);
			else {
				if ((cycle_count%CYCLE_RATE)==0)
					debug_printf ("Cycle count: %u Error count: %u\r", cycle_count, error_count);
				++cycle_count;
			}			

			if (lport_config.verify == 1) {
				debug_puts ("Press any key to continue or 'q' to exit...\n");
				cmd = debug_getchar();
				if (cmd == 'q' || cmd == 'Q')
					return;
			}
		}

		cmd = debug_peekchar ();

		if (lport_config.verify == 1) 
			return;

		if (cmd != -1) {
			debug_getchar ();
			return;
		}
	}
}

/**\~english
 * Select pair of memories and start test with DMA throught LP2-LP3
 *
 * \~russian
 * Выбор пары памятей и запуск теста с использованием DMA через LP2-LP3
 */
void
menu_dma23 (void)
{
	uint32_t check;
	char from;
	char to;
	char cmd;
	uint32_t error_count;
	uint32_t cycle_count = 0;

	debug_puts ("Select memories:\n\n");
try_again:
	debug_puts ("From: \n");
	debug_puts ("1. CRAM\n");
	debug_puts ("2. SRAM\n");
	debug_puts ("3. SDRAM\n");

	debug_puts ("Command: ");
	from = debug_getchar ();
	debug_printf ("%c\n", from);

	debug_puts ("\n\n To :\n");
	debug_puts ("1. CRAM\n");
	debug_puts ("2. SRAM\n");
	debug_puts ("3. SDRAM\n");

	debug_puts ("Command: ");
	to = debug_getchar ();
	debug_printf ("%c\n", to);

	if ((from > SDRAM) || (from < CRAM) || (to > SDRAM) || (to < CRAM)) {
		debug_puts ("Wrong input! Try again!\n\n");
		goto try_again;
	}
	
	for (;;) {
		for (check=0; check<TEST_COUNT; check++) {
			error_count = check_lport23_dma (from, to, check);
			
			if (lport_config.verify == 1)
				debug_printf ("Error count: %u\n", error_count);
			else {
				if ((cycle_count%CYCLE_RATE)==0)
					debug_printf ("Cycle count: %u Error count: %u\r", cycle_count, error_count);
				++cycle_count;
			}

			if (lport_config.verify == 1) {
				debug_puts ("Press any key to continue or 'q' to exit...\n");
				cmd = debug_getchar();
				if (cmd == 'q' || cmd == 'Q')
					return;
			}
		}

		cmd = debug_peekchar ();

		if (lport_config.verify == 1) 
			return;

		if (cmd != -1) {
			debug_getchar ();
			return;
		}
		
	}
}

/**\~english
 * Start test without DMA throught LP0-LP1
 *
 * \~russian
 * Запуск теста без использования DMA через LP0-LP1
 */
uint32_t 
check_lport01_ram (char from, char to, uint32_t check)
{
	uint32_t *start_mem_read = 0;
	uint32_t *start_mem_write = 0;
	uint32_t idx;
	uint32_t recv,send;
	uint32_t error_counter;
	
	if (from == CRAM)
		start_mem_read = in_array_cram;
	if (from == SRAM)
		start_mem_read = in_array_sram;
	if (from == SDRAM)
                start_mem_read = in_array_sdram;

	if (to == CRAM)
                start_mem_write = out_array_cram;
        if (to == SRAM)
                start_mem_write = out_array_sram;
        if (to == SDRAM)
                start_mem_write = out_array_sdram;

	prepare_test_array (start_mem_read, block_size, check);

	error_counter = 0;
        for (idx=0; idx<block_size; idx++) {
		
		send = *(volatile uint32_t*) (start_mem_read + idx);

		MC_LTX (0) = send;

		if (lport_config.verify == 1)
	                debug_printf ("Send 0x%08X\t", send);

	        while (!((MC_LCSR (1)) & 0x10));
        	recv = MC_LRX (1);

		*(volatile uint32_t*) (start_mem_write + idx) = recv;

		if (recv != send) {
			error_counter++;
		}
		if (lport_config.verify == 1)
	                debug_printf ("Receive 0x%08X\n", recv);
        }

	return error_counter;
}

/**\~english
 * Start test without DMA throught LP2-LP3
 *
 * \~russian
 * Запуск теста без использования DMA через LP2-LP3
 */
uint32_t 
check_lport23_ram (char from, char to, uint32_t check)
{
	uint32_t *start_mem_read = 0;
	uint32_t *start_mem_write = 0;
	uint32_t idx;
	uint32_t recv,send;
	uint32_t error_counter;
	
	if (from == CRAM)
		start_mem_read = in_array_cram;
	if (from == SRAM)
		start_mem_read = in_array_sram;
	if (from == SDRAM)
                start_mem_read = in_array_sdram;

	if (to == CRAM)
                start_mem_write = out_array_cram;
        if (to == SRAM)
                start_mem_write = out_array_sram;
        if (to == SDRAM)
                start_mem_write = out_array_sdram;

	prepare_test_array (start_mem_read, block_size, check);

	error_counter = 0;
        for (idx=0; idx<block_size; idx++) {
		
		send = *(volatile uint32_t*) (start_mem_read + idx);

		MC_LTX (2) = send;

		if (lport_config.verify == 1)
	                debug_printf ("Send 0x%08X\t", send);

	        while (!((MC_LCSR (3)) & 0x10));
        	recv = MC_LRX (3);

		*(volatile uint32_t*) (start_mem_write + idx) = recv;

		if (recv != send) {
			error_counter++;
		}
		if (lport_config.verify == 1)
	                debug_printf ("Receive 0x%08X\n", recv);
        }

	return error_counter;
}

/**\~english
 * Prepare input data for test and check result
 *
 * \~russian
 * Подготовка входных данных для теста и проверка результата
 */
uint32_t 
check_lport01_dma (char from, char to, uint32_t check)
{
        uint32_t idx;
        uint32_t error_counter = 0;
	uint32_t *in = 0;
	uint32_t *out = 0;

	test_lport01_dma (&in, &out, from, to, check);

	while( !(MC_CSR_LPCH(1) & 0x8000) );

	if (lport_config.verify == 1)
		for (idx = 0; idx < block_size; idx++) {
			debug_printf ("Send 0x%08X Receive 0x%08X\n", in[idx], out[idx]);
		}

	for (idx = 0; idx < block_size; idx++) {
		if (in[idx] != out[idx])
			error_counter++;
	}

	return error_counter;
}

/**\~english
 * Prepare input data for test and check result
 *
 * \~russian
 * Подготовка входных данных для теста и проверка результата
 */
uint32_t 
check_lport23_dma (char from, char to, uint32_t check)
{
        uint32_t idx;
        uint32_t error_counter = 0;
	uint32_t *in = 0;
	uint32_t *out = 0;

	test_lport23_dma (&in, &out, from, to, check);

	while( !(MC_CSR_LPCH(3) & 0x8000) );

	if (lport_config.verify == 1)
		for (idx = 0; idx < block_size; idx++) {
			debug_printf ("Send 0x%08X Receive 0x%08X\n", in[idx], out[idx]);
		}

	for (idx = 0; idx < block_size; idx++) {
		if (in[idx] != out[idx])
			error_counter++;
	}

	return error_counter;
}

/**\~english
 * Select pair of memories and start test without DMA throught LP0-LP1
 *
 * \~russian
 * Выбираем пару памятей и запуск теста без DMA через LP0-LP1
 */
void 
test_lport01_ram (void)
{
	char from, to;
	uint32_t idx;
	uint32_t cmd;
	uint32_t error_count;
	uint32_t cycle_count = 0;	

	if (lport_config.dir == 0) {
		MC_LCSR (1) = (lport_config.dir << MC_LCSR_LTRAN) |
			(lport_config.clk << MC_LCSR_LCLK_RATE) | 
			(lport_config.size << MC_LCSR_LDW) | 
			(1 << MC_LCSR_LEN);

		MC_LCSR (0) = ((!lport_config.dir) << MC_LCSR_LTRAN) |
			(lport_config.clk << MC_LCSR_LCLK_RATE) |
			(lport_config.size << MC_LCSR_LDW) |
			(1 << MC_LCSR_LEN);
	}
	else {
		MC_LCSR (0) = (lport_config.dir << MC_LCSR_LTRAN) |
			(lport_config.clk << MC_LCSR_LCLK_RATE) |
			(lport_config.size << MC_LCSR_LDW) |
			(1 << MC_LCSR_LEN);

		MC_LCSR (1) = ((!lport_config.dir) << MC_LCSR_LTRAN) |
			(lport_config.clk << MC_LCSR_LCLK_RATE) |
			(lport_config.size << MC_LCSR_LDW) |
			(1 << MC_LCSR_LEN);
	}

	debug_puts ("\n\nTesting LPort0 <=> LPort1\n");
try_again:
	debug_puts ("From: \n");
	debug_puts ("1. CRAM\n");
	debug_puts ("2. SRAM\n");
	debug_puts ("3. SDRAM\n");

	debug_puts ("Command: ");
	from = debug_getchar ();
	debug_printf ("%c\n", from);

	debug_puts ("\n\n To :\n");
	debug_puts ("1. CRAM\n");
	debug_puts ("2. SRAM\n");
	debug_puts ("3. SDRAM\n");

	debug_puts ("Command: ");
	to = debug_getchar ();
	debug_printf ("%c\n", to);

	if ((from > SDRAM) || (from < CRAM) || (to > SDRAM) || (to < CRAM)) {
		debug_puts ("Wrong input! Try again!\n\n");
		goto try_again;
	}

	for (;;) {
		for (idx=0; idx<TEST_COUNT; idx++) {
			error_count = check_lport01_ram (from, to, idx);

			if (lport_config.verify == 1)
				debug_printf ("Error count: %u\n", error_count);
			else {
				if ((cycle_count%CYCLE_RATE)==0)
					debug_printf ("Cycle count: %u Error count: %u\r", cycle_count, error_count);
				++cycle_count;
			}

			if (lport_config.verify == 1) {
				debug_puts ("Press any key to continue or 'q' to exit...\n");
				cmd = debug_getchar();
				if (cmd == 'q' || cmd == 'Q')
					return;
			}
		}
		
		cmd = debug_peekchar ();

		if (lport_config.verify == 1) 
			return;

		if (cmd != -1) {
			debug_getchar ();
			return;
		}
	}
}

/**\~english
 * Select pair of memories and start test without DMA throught LP2-LP3
 *
 * \~russian
 * Выбираем пару памятей и запуск теста без DMA через LP2-LP3
 */
void 
test_lport23_ram (void)
{
	char from, to;
	uint32_t idx;
	uint32_t cmd;
	uint32_t error_count;
	uint32_t cycle_count = 0;

	if (lport_config.dir == 0) {
		MC_LCSR (3) = (lport_config.dir << MC_LCSR_LTRAN) |
			(lport_config.clk << MC_LCSR_LCLK_RATE) | 
			(lport_config.size << MC_LCSR_LDW) | 
			(1 << MC_LCSR_LEN);

		MC_LCSR (2) = ((!lport_config.dir) << MC_LCSR_LTRAN) |
			(lport_config.clk << MC_LCSR_LCLK_RATE) |
			(lport_config.size << MC_LCSR_LDW) |
			(1 << MC_LCSR_LEN);
	}
	else {
		MC_LCSR (2) = (lport_config.dir << MC_LCSR_LTRAN) |
			(lport_config.clk << MC_LCSR_LCLK_RATE) |
			(lport_config.size << MC_LCSR_LDW) |
			(1 << MC_LCSR_LEN);

		MC_LCSR (3) = ((!lport_config.dir) << MC_LCSR_LTRAN) |
			(lport_config.clk << MC_LCSR_LCLK_RATE) |
			(lport_config.size << MC_LCSR_LDW) |
			(1 << MC_LCSR_LEN);
	}

	debug_puts ("\n\nTesting LPort2 <=> LPort3\n");
try_again:
	debug_puts ("From: \n");
	debug_puts ("1. CRAM\n");
	debug_puts ("2. SRAM\n");
	debug_puts ("3. SDRAM\n");

	debug_puts ("Command: ");
	from = debug_getchar ();
	debug_printf ("%c\n", from);

	debug_puts ("\n\n To :\n");
	debug_puts ("1. CRAM\n");
	debug_puts ("2. SRAM\n");
	debug_puts ("3. SDRAM\n");

	debug_puts ("Command: ");
	to = debug_getchar ();
	debug_printf ("%c\n", to);

	if ((from > SDRAM) || (from < CRAM) || (to > SDRAM) || (to < CRAM)) {
		debug_puts ("Wrong input! Try again!\n\n");
		goto try_again;
	}

	for (;;) {
		for (idx=0; idx<TEST_COUNT; idx++) {
			error_count = check_lport23_ram (from, to, idx);

			if (lport_config.verify == 1)
				debug_printf ("Error count: %u\n", error_count);
			else {
				if ((cycle_count%CYCLE_RATE)==0)
					debug_printf ("Cycle count: %u Error count: %u\r", cycle_count, error_count);
				++cycle_count;
			}

			if (lport_config.verify == 1) {
				debug_puts ("Press any key to continue or 'q' to exit...\n");
				cmd = debug_getchar();
				if (cmd == 'q' || cmd == 'Q')
					return;
			}
		}
		cmd = debug_peekchar ();

		if (lport_config.verify == 1) 
			return;

		if (cmd != -1) {
			debug_getchar ();
			return;
		}
	}
}

/**\~english
 * Start all tests
 *
 * \~russian
 * Запуск всех тестов
 */
void 
test_complex (void)
{
	char cmd;
	char mem_x;
	char mem_y;
	uint32_t idx;
	uint32_t error_count = 0;
	uint32_t error_count_total = 0;
	uint32_t cycle_count = 0;

	if (lport_config.dir == 0) {
		MC_LCSR (1) = (lport_config.dir << MC_LCSR_LTRAN) |
			(lport_config.clk << MC_LCSR_LCLK_RATE) | 
			(lport_config.size << MC_LCSR_LDW) | 
			(1 << MC_LCSR_LEN);

		MC_LCSR (0) = ((!lport_config.dir) << MC_LCSR_LTRAN) |
			(lport_config.clk << MC_LCSR_LCLK_RATE) |
			(lport_config.size << MC_LCSR_LDW) |
			(1 << MC_LCSR_LEN);
	}
	else {
		MC_LCSR (0) = (lport_config.dir << MC_LCSR_LTRAN) |
			(lport_config.clk << MC_LCSR_LCLK_RATE) |
			(lport_config.size << MC_LCSR_LDW) |
			(1 << MC_LCSR_LEN);

		MC_LCSR (1) = ((!lport_config.dir) << MC_LCSR_LTRAN) |
			(lport_config.clk << MC_LCSR_LCLK_RATE) |
			(lport_config.size << MC_LCSR_LDW) |
			(1 << MC_LCSR_LEN);
	}

	if (lport_config.dir == 0) {
		MC_LCSR (3) = (lport_config.dir << MC_LCSR_LTRAN) |
			(lport_config.clk << MC_LCSR_LCLK_RATE) | 
			(lport_config.size << MC_LCSR_LDW) | 
			(1 << MC_LCSR_LEN);

		MC_LCSR (2) = ((!lport_config.dir) << MC_LCSR_LTRAN) |
			(lport_config.clk << MC_LCSR_LCLK_RATE) |
			(lport_config.size << MC_LCSR_LDW) |
			(1 << MC_LCSR_LEN);
	}
	else {
		MC_LCSR (2) = (lport_config.dir << MC_LCSR_LTRAN) |
			(lport_config.clk << MC_LCSR_LCLK_RATE) |
			(lport_config.size << MC_LCSR_LDW) |
			(1 << MC_LCSR_LEN);

		MC_LCSR (3) = ((!lport_config.dir) << MC_LCSR_LTRAN) |
			(lport_config.clk << MC_LCSR_LCLK_RATE) |
			(lport_config.size << MC_LCSR_LDW) |
			(1 << MC_LCSR_LEN);
	}

	for (;;) {
		for (idx=0; idx<TEST_COUNT; idx++) {
			if (lport_config.dma == 0) {

				for (mem_y = CRAM; mem_y <= SDRAM; mem_y++) {
					for (mem_x = CRAM; mem_x <= SDRAM; mem_x++) {

						if (lport_config.verify == 1) {
							if (mem_y == CRAM)
								debug_puts ("CRAM => ");
							if (mem_y == SRAM)
								debug_puts ("SRAM => ");
							if (mem_y == SDRAM)
								debug_puts ("SDRAM => ");

							if (mem_x == CRAM)
								debug_puts ("CRAM\t");
							if (mem_x == SRAM)
								debug_puts ("SRAM\t");
							if (mem_x == SDRAM)
								debug_puts ("SDRAM\t");

							if (lport_config.pair == 0)
								debug_puts ("LP0<=>LP1\n");
							else
								debug_puts ("LP2<=>LP3\n");
						}

						if (lport_config.pair == 0)
							error_count += check_lport01_ram (mem_y, mem_x, idx);
						else
							error_count += check_lport23_ram (mem_y, mem_x, idx);

						if (lport_config.verify == 1) {
							debug_puts ("Press any key to continue or 'q' to exit...\n");
							cmd = debug_getchar();
							if (cmd == 'q' || cmd == 'Q')
								return;
						}
					}
				}
			}
			else {

				for (mem_y = CRAM; mem_y <= SDRAM; mem_y++) {
					for (mem_x = CRAM; mem_x <= SDRAM; mem_x++) {

						if (lport_config.verify == 1) {
							if (mem_y == CRAM)
								debug_puts ("CRAM => ");
							if (mem_y == SRAM)
								debug_puts ("SRAM => ");
							if (mem_y == SDRAM)
								debug_puts ("SDRAM => ");

							if (mem_x == CRAM)
								debug_puts ("CRAM\t");
							if (mem_x == SRAM)
								debug_puts ("SRAM\t");
							if (mem_x == SDRAM)
								debug_puts ("SDRAM\t");

							if (lport_config.pair == 0)
								debug_puts ("LP0<=>LP1\n");
							else
								debug_puts ("LP2<=>LP3\n");
						}

						if (lport_config.pair == 0)
							error_count += check_lport01_dma (mem_y, mem_x, idx);
						else
							error_count += check_lport23_dma (mem_y, mem_x, idx);

						if (lport_config.verify == 1) {
							debug_puts ("Press any key to continue or 'q' to exit...\n");
							cmd = debug_getchar();
							if (cmd == 'q' || cmd == 'Q')
								return;
						}
					}
				}
			}

			if (lport_config.verify == 1) {
				debug_printf ("\nError count: %u\n\n", error_count);
				error_count_total += error_count;
				error_count = 0;
			}
		}
	
		if (lport_config.verify == 1)
			debug_printf ("Total error count: %u\n", error_count);
		else {
			if ((cycle_count%CYCLE_RATE_COMPLEX)==0)
				debug_printf ("Cycle count: %u Error count: %u\r", cycle_count, error_count);
			++cycle_count;
		}

		cmd = debug_peekchar ();

		if (lport_config.verify == 1) 
			return;

		if (cmd != -1) {
			debug_getchar ();
			return;
		}
	}
}

/**\~english
 * Main menu
 *
 * \~russian
 * Главное меню
 */
void 
menu(void)
{
	char cmd;
	uint32_t number = 0;

	debug_puts ("\nStats:\n");
	
	if (lport_config.dir == 0) {
		if (lport_config.pair == 0)
			debug_puts ("Direction:\tLP0 => LP1\n");
		else
			debug_puts ("Direction:\tLP2 => LP3\n");
	}
	else {
		if (lport_config.pair == 0)
			debug_puts ("Direction:\tLP0 <= LP1\n");
		else
			debug_puts ("Direction:\tLP2 <= LP3\n");
	}

	if (lport_config.dma == 0)
		debug_puts ("DMA:\t\tdon't use\n");
	else
		debug_puts ("DMA:\t\tuse\n");

	if (lport_config.clk == 0)
		debug_puts ("Transfer speed:\tCLK/8\n");
	else
		debug_puts ("Transfer speed:\tCLK/4\n");

	if (lport_config.size == 0)
		debug_puts ("Transfer size:\t4\n");
	else
		debug_puts ("Transfer size:\t8\n");

	if (lport_config.verify == 0)
		debug_puts ("Verify:\t\toff\n");
	else
		debug_puts ("Verify:\t\ton\n");

	debug_printf ("Block size:\t%u\n", block_size);

	debug_puts ("\nMenu:\n");
	debug_puts ("1. Select pair\n");
	debug_puts ("2. Config\n");
	debug_puts ("3. Start tests\n");
	debug_puts ("4. Start complex test\n");
	
	debug_puts ("Command: ");
	cmd = debug_getchar ();
	debug_printf ("%c\n", cmd);

	if (cmd == '1') {
		debug_puts ("\nSelect pair:\n");
		if (lport_config.pair == 0)
			debug_puts ("Current pair: LP0, LP1\n\n");
		else
			debug_puts ("Current pair: LP2, LP3\n\n");			
		debug_puts ("1. LP0, LP1\n");
		debug_puts ("2. LP2, LP3\n");

		debug_puts ("Command: ");
		cmd = debug_getchar ();
		debug_printf ("%c\n", cmd);

		if (cmd == '1')
			lport_config.pair = 0;

		if (cmd == '2')
			lport_config.pair = 1;

		cmd = 0;
	}

	if (cmd == '2') {
config:
		debug_puts ("\nConfig:\n");
		if (lport_config.dir == 0) {
			if (lport_config.pair == 0)
				debug_puts ("1. Direction (LP0 => LP1)\n");
			else
				debug_puts ("1. Direction (LP2 => LP3)\n");
		}
		else {
			if (lport_config.pair == 0)
				debug_puts ("1. Direction (LP0 <= LP1)\n");
			else
				debug_puts ("1. Direction (LP2 <= LP3)\n");
		}

		if (lport_config.dma == 0)
			debug_puts ("2. Usage DMA (don't use)\n");
		else
			debug_puts ("2. Usage DMA (use)\n");

		if (lport_config.clk == 0)
			debug_puts ("3. Transfer speed (CLK/8)\n");
		else
			debug_puts ("3. Transfer speed (CLK/4)\n");

		if (lport_config.size == 0)
			debug_puts ("4. Transfer size (4)\n");
		else
			debug_puts ("4. Transfer size (8)\n");

		if (lport_config.verify == 0)
			debug_puts ("5. Verify (off)\n");
		else
			debug_puts ("5. Verify (on)\n");

		debug_printf ("6. Block size: %u (max size: %u)\n", block_size, BLOCK_SIZE_MAX);

		debug_puts ("7. Save and quit\n");

		debug_puts ("Command: ");
		cmd = debug_getchar ();
		debug_printf ("%c\n", cmd);

		if (cmd == '1')
			lport_config.dir = !lport_config.dir;

		if (cmd == '2')
			lport_config.dma = !lport_config.dma;

		if (cmd == '3')
			lport_config.clk = !lport_config.clk;

		if (cmd == '4')
			lport_config.size = !lport_config.size;

		if (cmd == '5')
			lport_config.verify = !lport_config.verify;

		if (cmd == '6') {
			debug_puts("\nEnter block size:\n");

			for (;;) {
				cmd = debug_getchar ();

				if ((!isdigit(cmd))&&(cmd != 0x8))
					break;

				if (cmd == 0x8) {
					number = number/10;
					debug_printf("    \r");
				}
				else {
					number = number*10 + (cmd - '0');
				}

				if (number > 9999)
					number = number/10;

				debug_printf("%u\r", number);
			}

			if (number > BLOCK_SIZE_MAX)
				number = BLOCK_SIZE_MAX;

			if (number == 0)
				number = BLOCK_SIZE_DEFAULT;
		
			block_size = number;

			if (in_array_cram) {
				mem_free (in_array_cram);
				in_array_cram = 0;
			}

			if (out_array_cram) {
				mem_free (out_array_cram);
				out_array_cram = 0;
			}

			if (in_array_sram) {
				mem_free (in_array_sram);
				in_array_sram = 0;
			}

			if (out_array_sram) {
				mem_free (out_array_sram);
				out_array_sram = 0;
			}

			if (in_array_sdram) {
				mem_free (in_array_sdram);
				in_array_sdram = 0;
			}

			if (out_array_sdram) {
				mem_free (out_array_sdram);
				out_array_sdram = 0;
			}

			in_array_cram  = (uint32_t*)mem_alloc (&pool_cram, block_size * sizeof (uint32_t));
			out_array_cram = (uint32_t*)mem_alloc (&pool_cram, block_size * sizeof (uint32_t));

			in_array_sram  = (uint32_t*)mem_alloc (&pool_sram, block_size * sizeof (uint32_t));
			out_array_sram = (uint32_t*)mem_alloc (&pool_sram, block_size * sizeof (uint32_t));

			in_array_sdram  = (uint32_t*)mem_alloc (&pool_sdram, block_size * sizeof (uint32_t));
			out_array_sdram = (uint32_t*)mem_alloc (&pool_sdram, block_size * sizeof (uint32_t));
			
			number = 0;
		}

		if (cmd == '7')
			goto config_finish;

		goto config;

config_finish:
		cmd = 0;
	}

	if (cmd == '3') {
		if (lport_config.pair == 0) {
			if (lport_config.dma == 0) {
				test_lport01_ram ();
			}
			else {
				menu_dma01 ();
			}
		}
		else {
			if (lport_config.dma == 0) {
				test_lport23_ram ();
			}
			else {
				menu_dma23 ();
			}
		}
	}

	if (cmd == '4') {
		test_complex ();
	}
}

void 
main_console (void *data)
{
	lport_config.pair = 0;
	lport_config.dir = 0;	
	lport_config.dma = 0;
	lport_config.clk = 0;
	lport_config.size = 0;
	lport_config.verify = 0;

	for (;;)
		menu ();
}

void 
uos_init(void)
{

	debug_printf ("\nTesting LPORT on MC-24EM board\n");
	debug_printf ("Generator %d.%d MHz, CPU clock %d.%d MHz\n",
		ELVEES_CLKIN/1000, ELVEES_CLKIN/100%10, KHZ/1000, KHZ/100%10);

	MC_CSCON3 = MC_CSCON_WS (8);

	MC_CSCON0 = MC_CSCON_E |
		MC_CSCON_T |
		MC_CSCON_W64 |
		MC_CSCON_CSBA (0x00000000) |
		MC_CSCON_CSMASK (0xF8000000);

	MC_CSCON1 = MC_CSCON_E |
                MC_CSCON_T |
                MC_CSCON_W64 |
                MC_CSCON_CSBA (0x04000000) |
                MC_CSCON_CSMASK (0xF8000000);

	MC_SDRCON = MC_SDRCON_INIT |
		MC_SDRCON_BL_PAGE |
		MC_SDRCON_RFR (64000000/8192, KHZ) |
		MC_SDRCON_PS_512;
	udelay (2);

	debug_printf ("\tCSR 	= 0x%08X\n", MC_CSR);
	debug_printf ("\tCSCON0	= 0x%08X\n", MC_CSCON0);
	debug_printf ("\tCSCON1	= 0x%08X\n", MC_CSCON1);
	debug_printf ("\tCSCON3	= 0x%08X\n", MC_CSCON3);
	debug_printf ("\tSDRCON	= 0x%08X\n", MC_SDRCON);

	mem_init (&pool_sdram, SDRAM_MEM_START, SDRAM_MEM_END);
	mem_init (&pool_cram, CRAM_MEM_START, CRAM_MEM_END);
	mem_init (&pool_sram, SRAM_MEM_START, SRAM_MEM_END);

	in_array_cram  = (uint32_t*)mem_alloc (&pool_cram, block_size * sizeof (uint32_t));
	out_array_cram = (uint32_t*)mem_alloc (&pool_cram, block_size * sizeof (uint32_t));

	in_array_sram  = (uint32_t*)mem_alloc (&pool_sram, block_size * sizeof (uint32_t));
	out_array_sram = (uint32_t*)mem_alloc (&pool_sram, block_size * sizeof (uint32_t));

	in_array_sdram  = (uint32_t*)mem_alloc (&pool_sdram, block_size * sizeof (uint32_t));
	out_array_sdram = (uint32_t*)mem_alloc (&pool_sdram, block_size * sizeof (uint32_t));

	task_create (main_console, 0, "console", 1,
		stack_console, sizeof (stack_console));
}
