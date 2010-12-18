/*
 * Тест микроконтроллера LAN91C111
 * Автор: Дубихин В.А.
 */
#include <runtime/lib.h>
#include <stream/stream.h>
#include <smc91c111/regs.h>

#include "eth.h"

#define TX_BYTES	64
#define DATA_PATTERN	0xbad0f00d

#define MAC_ADDR01	0x0123
#define MAC_ADDR23	0x4567
#define MAC_ADDR45	0x89ab

#define GOOD_PHY_ID1	0x0016
#define GOOD_PHY_ID2	0xF840

/* Изначально предполагаем, что заглушка Ethernet установлена. */
static unsigned eth_extloop_installed = 1;

static const unsigned char *EXT_LOOP_INSTALLED		= (unsigned char *)"установлена внешняя заглушка";
static const unsigned char *EXT_LOOP_NOT_INSTALLED	= (unsigned char *)"внешняя заглушка не установлена";

/*
 * Отображение параметров теста.
 */
void eth_show ()
{
	/* Установлена ли внешняя заглушка-шлейф. */
	if (eth_extloop_installed)
		printf (&debug, "%s", EXT_LOOP_INSTALLED);
	else
		printf (&debug, "%s", EXT_LOOP_NOT_INSTALLED);
}

/*
 * Установка конфигурации (программно)
 */
int eth_set_config (unsigned char * new_config)
{
	if (strcmp (new_config, EXT_LOOP_INSTALLED) == 0)
		eth_extloop_installed = 1;
	else if (strcmp (new_config, EXT_LOOP_NOT_INSTALLED) == 0)
		eth_extloop_installed = 0;
	else
		return 0;
	return 1;
}

static unsigned read_mii_register (unsigned phyaddr, unsigned phyreg)
{
	int old_bank, i, input_idx;
	unsigned mii_reg, phydata, mask, bits [64];

	int clk_idx = 0;			// 32 consecutive ones on MDO to establish sync
	for (i=0; i<32; ++i)
		bits [clk_idx++] = MII_MDOE | MII_MDO;
	bits [clk_idx++] = MII_MDOE;		// Start code <01>
	bits [clk_idx++] = MII_MDOE | MII_MDO;
	bits [clk_idx++] = MII_MDOE | MII_MDO;	// Read command <10>
	bits [clk_idx++] = MII_MDOE;

	mask = 0x10;				// Output the PHY address, msb first
	for (i=0; i<5; ++i) {
		bits [clk_idx++] = (phyaddr & mask) ?
			MII_MDOE | MII_MDO : MII_MDOE;
		mask >>= 1;
	}
	mask = 0x10;				// Output the phy register number, msb first
	for (i=0; i<5; ++i) {
		bits [clk_idx++] = (phyreg & mask) ?
			MII_MDOE | MII_MDO : MII_MDOE;
		mask >>= 1;
	}

	bits [clk_idx++] = 0;			// Tristate and turnaround
	input_idx = clk_idx;			// Input starts at this bit time
	for (i=0; i<16; ++i)			// Will input 16 bits
		bits [clk_idx++] = 0;
	bits [clk_idx++] = 0;			// Final clock bit

	old_bank = BS_REG;			// Save the current bank
	BS_REG = 3;				// Select bank 3
	mii_reg = MII_REG;			// Get the current MII register value
	mii_reg &= ~(MII_MDOE | MII_MCLK |	// Turn off all MII Interface bits
		MII_MDI | MII_MDO);
	for (i=0; i<64; ++i) {			// Clock all 64 cycles
		MII_REG = mii_reg | bits [i];	// Clock Low - output data
		udelay (50);
		MII_REG = mii_reg | bits [i] | MII_MCLK; // Clock Hi - input data
		udelay (50);
		bits [i] |= MII_REG & MII_MDI;
	}
	MII_REG = mii_reg;			// Return to idle state
	udelay (50);
	BS_REG = old_bank;			// Restore original bank select

	phydata = 0;				// Recover input data
	for (i=0; i<16; ++i) {
		phydata <<= 1;
		if (bits [input_idx++] & MII_MDI)
			phydata |= 1;
	}
	return phydata;
}

static void write_mii_register (unsigned phyaddr, unsigned phyreg,
	unsigned phydata)
{
	int old_bank, i;
	unsigned mask, mii_reg, bits [65];

	int clk_idx = 0;			// 32 consecutive ones on MDO to establish sync
	for (i = 0; i < 32; ++i)
		bits [clk_idx++] = MII_MDOE | MII_MDO;
	bits [clk_idx++] = MII_MDOE;		// Start code <01>
	bits [clk_idx++] = MII_MDOE | MII_MDO;
	bits [clk_idx++] = MII_MDOE;		// Write command <01>
	bits [clk_idx++] = MII_MDOE | MII_MDO;

	mask = 0x10;				// Output the PHY address, msb first
	for (i=0; i<5; ++i) {
		bits [clk_idx++] = (phyaddr & mask) ?
			MII_MDOE | MII_MDO : MII_MDOE;
		mask >>= 1;
	}
	mask = 0x10;				// Output the phy register number, msb first
	for (i=0; i<5; ++i) {
		bits [clk_idx++] = (phyreg & mask) ?
			MII_MDOE | MII_MDO : MII_MDOE;
		mask >>= 1;
	}
	bits [clk_idx++] = 0;			// Tristate and turnaround (2 bit times)
	bits [clk_idx++] = 0;
	mask = 0x8000;				// Write out 16 bits of data, msb first
	for (i=0; i<16; ++i) {
		bits [clk_idx++] = (phydata & mask) ?
			MII_MDOE | MII_MDO : MII_MDOE;
		mask >>= 1;
	}
	bits [clk_idx++] = 0;			// Final clock bit (tristate)

	old_bank = BS_REG;			// Save the current bank
	BS_REG = 3;				// Select bank 3
	mii_reg = MII_REG;			// Get the current MII register value
	mii_reg &= ~(MII_MDOE | MII_MCLK |	// Turn off all MII Interface bits
		MII_MDI | MII_MDO);
	for (i=0; i<65; ++i) {			// Clock all cycles
		MII_REG = mii_reg | bits [i];	// Clock Low - output data
		udelay (50);
		MII_REG = mii_reg | bits [i] | MII_MCLK; // Clock Hi - input data
		udelay (50);
		bits [i] |= MII_REG & MII_MDI;
	}
	MII_REG = mii_reg;			// Return to idle state
	udelay (50);
	BS_REG = old_bank;			// Restore original bank select
}

/*
 * Приведение PHY в исходное состояние.
 * Возврат 0, если всё хорошо.
 */
static int phy_configure (int intloop, int verbose)
{
	int timeout;

	/* Сброс PHY, ждём 3 секунды. */
	write_mii_register (0, PHY_CNTL_REG, PHY_CNTL_RST);
	for (timeout=60; ; timeout--) {
		if (! (read_mii_register (0, PHY_CNTL_REG) & PHY_CNTL_RST))
			break;
		if (timeout <= 0) {
			printf (&debug, "Контроллер Ethernet: сброс PHY не завершился\n");
			return 1;
		}
		mdelay (50);
	}

	/* Configure the Receive/Phy Control register */
	unsigned rpc_cur_mode = (RPC_LED_100 << RPC_LSXA_SHFT) |
		(RPC_LED_FD << RPC_LSXB_SHFT) | RPC_SPEED | RPC_DPLX;
	BS_REG = 0;
	RPC_REG = rpc_cur_mode;

	// Copy our capabilities from PHY_STAT_REG to PHY_AD_REG
	unsigned my_phy_caps = read_mii_register (0, PHY_STAT_REG);
	unsigned my_ad_caps = PHY_AD_CSMA;	// I am CSMA capable
	if (my_phy_caps & PHY_STAT_CAP_T4)
		my_ad_caps |= PHY_AD_T4;
	if (my_phy_caps & PHY_STAT_CAP_TXF)
		my_ad_caps |= PHY_AD_TX_FDX;
	if (my_phy_caps & PHY_STAT_CAP_TXH)
		my_ad_caps |= PHY_AD_TX_HDX;
	if (my_phy_caps & PHY_STAT_CAP_TF)
		my_ad_caps |= PHY_AD_10_FDX;
	if (my_phy_caps & PHY_STAT_CAP_TH)
		my_ad_caps |= PHY_AD_10_HDX;

	write_mii_register (0, PHY_AD_REG, my_ad_caps);

	// Restart auto-negotiation process in order to advertise my caps
	unsigned cntl = PHY_CNTL_ANEG_EN | PHY_CNTL_SPEED /*| PHY_CNTL_ANEG_RST*/;
	if (intloop)
		cntl |= PHY_CNTL_LPBK;
	write_mii_register (0, PHY_CNTL_REG, cntl);

	// Wait for the auto-negotiation to complete.
	for (timeout=10; ; timeout--) {
		unsigned status = read_mii_register (0, PHY_STAT_REG);
		//printf (&debug, "status = %b\n", status, PHY_STAT_BITS);
		if (intloop || (status & PHY_STAT_ANEG_ACK) ||
		    (status & PHY_STAT_LINK)) {
			/* Есть соединение. */
			break;
		}
		if (timeout <= 0) {
			printf (&debug, "Контроллер Ethernet: нет соединения\n");
			return 1;
		}
		// Restart auto-negotiation if remote fault
		if (status & PHY_STAT_REM_FLT) {
			write_mii_register (0, PHY_CNTL_REG,
				cntl | PHY_CNTL_SPEED | PHY_CNTL_DPLX);
		}
		mdelay (50);
	}

	// Set our sysctl parameters to match auto-negotiation results
	unsigned status_output = read_mii_register (0, PHY_INT_REG);
	if (verbose >= 0)
		printf (&debug, "Контроллер Ethernet: соединение %d Мбит/сек, %s.\n",
			(status_output & PHY_INT_SPDDET) ? 100 : 10,
			(status_output & PHY_INT_DPLXDET) ?
			"полный дуплекс" : "полудуплекс");

	// Re-Configure the Receive/Phy Control register
	if (! (status_output & PHY_INT_SPDDET))
		rpc_cur_mode &= ~RPC_SPEED;
	if (! (status_output & PHY_INT_DPLXDET))
		rpc_cur_mode &= ~RPC_DPLX;
	RPC_REG = rpc_cur_mode;
	return 0;
}

/*
 * в случае необходимости можно добавить 3 атрибут-указатель на пересыламые данные
 * сформируем и отправим пакет
 */
static int form_packet ()
{
	/*
	 * Выделяем память для пересылаемого пакета.
	 * Проверим прерывание ALLOC_INT.
	 */
	BS_REG = 2;
	while (MMU_CMD_REG & MC_BUSY)
		continue;
	MMU_CMD_REG = MC_ALLOC | 1;
	while (MMU_CMD_REG & MC_BUSY)
		continue;
	if (! (INT_REG & IM_ALLOC_INT)) {
		printf (&debug, "Ошибка: нет прерывания ALLOC_INT\n");
		return 1;
	}

	/*
	 * Сформируем пакет для передачи.
	 */
	PN_REG = AR_REG & 0x7F;		/* обнулим 8 бит */
	PTR_REG = PTR_AUTOINC;
	DATA32_REG = (TX_BYTES + 6) << 16;
	DATA32_REG = MAC_ADDR01 | MAC_ADDR23 << 16;
	DATA32_REG = MAC_ADDR45 | MAC_ADDR01 << 16;
	DATA32_REG = MAC_ADDR23 | MAC_ADDR45 << 16;
	unsigned n;
	for (n=0; n<(TX_BYTES-12)/4; n++)
		DATA32_REG = DATA_PATTERN;
	DATA16_REG = 0;

	/* Пакет сформирован, отсылаем. */
	BS_REG = 2;
	MMU_CMD_REG = MC_ENQUEUE;
	while (MMU_CMD_REG & MC_BUSY)
		continue;
	return 0;
}

static int check_packet (int verbose)
{
	unsigned n;

	//проверим пришел ли пакет
	BS_REG = 2;
	for (n=100; n>0; n--) {
		if (INT_REG & IM_RCV_INT)
			break;
		udelay (50);
	}
	if (! n) {
		printf (&debug,"Ошибка: пакет не принят\n");
		return 1;
	}
        unsigned packet_number = FIFO_REG;
        if (packet_number & FIFO_REMPTY) {
		printf (&debug,"Ошибка: принятый пакет отсутствует в FIFO\n");
		return 1;
        }
	packet_number >>= 8;

        /* Начинаем вычитывать пакет. */
        PTR_REG = PTR_READ | PTR_RCV | PTR_AUTOINC;

        /* Первые два 16-битных слова - статус и длина пакета. */
	unsigned status = DATA32_REG;
	unsigned rx_bytes = (status >> 16) & 0x07ff;
	status &= 0xffff;
//	if (verbose > 0)
//		printf (&debug, "Номер пакета %d, статус приёма %04x, длина %d байтов.\n",
//			packet_number, status, rx_bytes);

	unsigned data [TX_BYTES/4];
	for (n=0; n<TX_BYTES/4; n++)
		data[n] = DATA32_REG;
//	if (verbose > 0)
//		printf (&debug, "Данные: %08x-%08x-%08x-%08x...\n",
//			data[0], data[1], data[2], data[3]);

	int errors = 0;
        if (status & (RS_ALGNERR | RS_BADCRC | RS_TOOLONG | RS_TOOSHORT)) {
		printf (&debug,"Ошибка: плохой статус приёма\n");
		errors++;
	} else if (rx_bytes != TX_BYTES + 6) {
		printf (&debug,"Ошибка: неверная длина принятого пакета\n");
		errors++;
	} else if (data[0] != (MAC_ADDR01 | MAC_ADDR23 << 16) ||
	    data[1] != (MAC_ADDR45 | MAC_ADDR01 << 16) ||
	    data[2] != (MAC_ADDR23 | MAC_ADDR45 << 16) ||
	    data[3] != DATA_PATTERN) {
		printf (&debug, "Ошибка данных: передано %08X-%08X-%08X-%08X,\n",
			MAC_ADDR01 | MAC_ADDR23 << 16,
			MAC_ADDR45 | MAC_ADDR01 << 16,
			MAC_ADDR23 | MAC_ADDR45 << 16, DATA_PATTERN);
		printf (&debug, "               принято %08X-%08X-%08X-%08X\n",
			data[0], data[1], data[2], data[3]);
		errors++;
	}
	MMU_CMD_REG = MC_RELEASE;
	while (MMU_CMD_REG & MC_BUSY)
		continue;
	return errors;
}

inline int eth_check_bytes_in_mcastl (unsigned bytenum, unsigned value)
{
	/* Задаём начальное значение (шаблон) тестируемого слова. */
	unsigned expected = 0x12345678;

	/* Запись слова. */
	MCASTL_REG = expected;

	/* Меняем значение тестируемого байта в слове и в шаблоне. */
	((volatile unsigned char*) &MCASTL_REG) [bytenum] = value;
	((unsigned char*) &expected) [bytenum] = value;

	/* Чтение и проверка cлова. */
	unsigned word = MCASTL_REG;
	if (word != expected) {
		printf (&debug, "Ошибка байта %d в регистре MCASTL: прочитано %08X, ожидается %08X\n",
			bytenum, word, expected);
		return 1;
	}
	return 0;
}

static int eth_test_bytes ()
{
	int i, nerrors = 0;

	for (i=0; i<4; ++i) {
		nerrors += eth_check_bytes_in_mcastl (i, 0x00);
		nerrors += eth_check_bytes_in_mcastl (i, 0x55);
		nerrors += eth_check_bytes_in_mcastl (i, 0xaa);
		nerrors += eth_check_bytes_in_mcastl (i, 0xff);
	}
	return nerrors;
}

/*
 * Запуск проверки контроллера Ethernet.
 * Возвращает 0 в случае успеха.
 *
 * Параметр 'verbose' управляет объёмом тестирования и количеством
 * выдаваемых диагностических сообщений.
 * Если verbose < 0 - выполняем только быстрые тесты (доли секунды),
 * выдаём одну-две строки.
 * Если verbose >= 0 - выполняем подробные тесты (несколько секунд).
 * Если verbose > 0 - выдаём подробную информацию, до нескольких сот строк.
 *
 * Если параметр 'stop_on_error' не 0, необходимо при первой же
 * обнаруженной ошибке прервать тестирование.
 */
int eth_run (int verbose, int stop_on_error)
{
	int errors = 0;

	if (verbose >= 0)
		printf (&debug, "Контроллер Ethernet: проверка регистров.\n");

	/* Проверка старших разрядов регистра BSR. */
	if (verbose > 0)
		printf (&debug, "Контроллер Ethernet: проверка регистра BSR.\n");
	unsigned bsr = BS_REG;
	if (verbose > 0)
		printf (&debug, "Регистр BSR: прочитано %04X\n", bsr);
	if ((bsr & 0xff00) != 0x3300) {
		printf (&debug, "Ошибка старших разрядов регистра BSR: прочитано %04X, ожидается 33xx\n",
			bsr);
		if (stop_on_error)
			return 1;
		++errors;
	}

	/* Проверка младших разрядов регистра BSR. */
	unsigned n;
	for (n = 0; n<8; n++) {
		BS_REG = n;
		unsigned rval = BS_REG;
		if (verbose > 0)
			printf (&debug, "Регистр BSR: записано %04X, прочитано %04X\n",
				n, rval);
		if (rval != (n | 0x3300)) {
			printf (&debug, "Ошибка записи в регистр BSR: записано %08X, прочитано %08X\n",
				n, rval);
			if (stop_on_error)
				return 1;
			++errors;
		}
	}

	/* Проверка регистра REV. */
	if (verbose > 0)
		printf (&debug, "Контроллер Ethernet: проверка регистра REV.\n");
	BS_REG = 3;
	unsigned rev = REV_REG;
	if (verbose > 0)
		printf (&debug, "Регистр REV: прочитано %04X\n", rev);
	if (rev != 0x3392) {
		printf (&debug, "Ошибка регистра REV: прочитано %04X, ожидается 3392\n",
			rev);
		if (stop_on_error)
			return 1;
		++errors;
	}

	/* Сброс и минимальная инициализация. */
	BS_REG = 0;
	RCR_REG = RCR_SOFTRST;
	BS_REG = 1;
	CONFIG_REG = CONFIG_NO_WAIT | CONFIG_EPH_POWER_EN;
	mdelay (10);
	BS_REG = 0;
	TCR_REG = 0;
	RCR_REG = 0;
	BS_REG = 2;
	IM_REG = 0;
	INT_REG = INT_REG;	/* Гашение прерываний. */

	/* Проверка регистров MCASTL/MCASTH бегущей единицей. */
	if (verbose > 0)
		printf (&debug, "Контроллер Ethernet: проверка регистров MCASTL/MCASTH.\n");
	BS_REG = 3;
	for (n=1; n; n<<=1) {
		MCASTL_REG = n;
		MCASTH_REG = ~n;
		unsigned vl = MCASTL_REG;
		unsigned vh = MCASTH_REG;
		if (verbose > 0)
			printf (&debug, "Регистр MCAST: записано %08X/%08X, прочитано %08X/%08X\n",
				n, ~n, vl, vh);
		if (n != vl) {
			printf (&debug, "Ошибка записи в регистр MCASTL: записано %08X, прочитано %08X\n",
				n, vl);
			if (stop_on_error)
				return 1;
			++errors;
		}
		if (~n != vh) {
			printf (&debug, "Ошибка записи в регистр MCASTH: записано %08X, прочитано %08X\n",
				~n, vh);
			if (stop_on_error)
				return 1;
			++errors;
		}
	}

	/* Проверка побайтовой записи в MCASTL */
	if (verbose > 0)
		printf (&debug, "Проверка побайтовой записи в регистр MCASTL.\n");
	if (eth_test_bytes () != 0) {
		if (stop_on_error)
			return 1;
		++errors;
	}

	/* Проверка идентификатора встроенного PHY. */
	unsigned phy_id1 = read_mii_register (0, PHY_ID1_REG);
	unsigned phy_id2 = read_mii_register (0, PHY_ID2_REG) & 0xFFF0;

	if (phy_id1 != GOOD_PHY_ID1 || phy_id2 != GOOD_PHY_ID2) {
		printf (&debug, "Ошибка идентификатора PHY: прочитано %04X/%04X, ожидается %04X/%04X\n",
			phy_id1, phy_id2, GOOD_PHY_ID1, GOOD_PHY_ID2);
		if (stop_on_error)
			return 1;
		++errors;
	}
	if (verbose >= 0)
		printf (&debug, "Контроллер Ethernet: встроенный PHY LAN83C183.\n");

	/* Настраиваем режимы PHY.
	 * Если стартовый режим или нет внешней заглушки -
	 * включаем внутренний шлейф (PHY loop). */
	int intloop = (verbose < 0) ? 1 : ! eth_extloop_installed;
	if (phy_configure (intloop, verbose) != 0) {
		if (stop_on_error)
			return 1;
		++errors;
	}

	/* Проверим, что нет прерывания. */
	unsigned cause = mips32_read_c0_register (C0_CAUSE);
	if (cause & CA_IP_IRQ2) {
		printf (&debug, "Контроллер Ethernet: неожиданное прерывание /IRQ2.\n");
		printf (&debug, "    CAUSE = %08X\n", cause);
		if (stop_on_error)
			return 1;
		++errors;
	}

	/* Сконфигурируем чип на передачу и приём пакетов. */
	if (verbose >= 0)
		printf (&debug, "Контроллер Ethernet: передача и приём пакетов с %s шлейфом.\n",
			intloop ? "внутренним" : "внешним");
	BS_REG = 1;
	ADDR0_REG = MAC_ADDR01;
	ADDR1_REG = MAC_ADDR23;
	ADDR2_REG = MAC_ADDR45;
	BS_REG = 0;
	TCR_REG = TCR_ENABLE | TCR_PAD_EN | TCR_FDUPLX;
	RCR_REG = RCR_RXEN | RCR_STRIP_CRC;
	BS_REG = 1;
	CTL_REG |= CTL_AUTO_RELEASE;
	BS_REG = 2;
	IM_REG = IM_ALLOC_INT;

	unsigned nloops = verbose<0 ? 1 : 10000;
	for (n=0; n<nloops; n++) {
		/* Отправка пакета. */
		if (form_packet () != 0 && stop_on_error)
			return 1;

		/* Проверим, что есть прерывание. */
		cause = mips32_read_c0_register (C0_CAUSE);
		if (! (cause & CA_IP_IRQ2)) {
			BS_REG = 2;
			printf (&debug, "Контроллер Ethernet: отсутствует прерывание /IRQ2.\n");
			printf (&debug, "    CAUSE=%08X, INT=%02X\n", cause, INT_REG);
			if (stop_on_error)
				return 1;
			++errors;
		}
		/* Приём и проверка пакета */
		if (check_packet (verbose) != 0) {
			if (verbose >= 0)
				printf (&debug, "Ошибка приёма пакета с номером %d.\n", n);
			if (stop_on_error)
				return 1;
			++errors;
			nloops = 1;
		}
		if (escape_pressed ()) {
			/* Escape - останавливаем тест. */
			printf (&debug, "Тест прерван.\n");
			return 0;
		}
	}
	if (! errors)
		puts (&debug, "Контроллер Ethernet: в порядке.\n");
	return errors;
}

static void bsr_read_test ()
{
	printf (&debug, "Чтение адреса %08X. ", &BS_REG);
	printf (&debug, "Для прекращения нажмите <Esc>\n");

	unsigned i, val;
	for (i=0; ; ++i) {
		val = BS_REG;
		printf (&debug, "\rРегистр BSR: прочитано %04X (%u) ", val, i);

		if (escape_pressed ()) {
			/* Escape - останавливаем тест. */
			getchar (&debug);
			printf (&debug, "\nВыполнение прервано клавишей <Esc>\n");
			break;
		}
	}
}

void bsr_write_test ()
{
	printf (&debug, "Запись-чтение адреса %08X. ", &BS_REG);
	printf (&debug, "Для прекращения нажмите <Esc>\n");

	unsigned i;
	for (i = 0; ; i = (i+1) & 7) {
		BS_REG = i;
		unsigned rval = BS_REG;
		printf (&debug, "\rЗаписано %04X прочитано %04X ",
			i, rval);

		if (escape_pressed ()) {
			/* Escape - останавливаем тест. */
			getchar (&debug);
			printf (&debug, "\nВыполнение прервано клавишей <Esc>\n");
			break;
		}
	}
}

static void show_regs ()
{
	/* Регистры банка 0 */
	BS_REG = 0;
	printf (&debug, "  TCR        = %04X\n", TCR_REG);
	printf (&debug, "  EPH_STATUS = %04X\n", EPH_STATUS_REG);
	printf (&debug, "  RCR        = %04X\n", RCR_REG);
	printf (&debug, "  COUNTER    = %04X\n", COUNTER_REG);
	printf (&debug, "  MIR        = %04X\n", MIR_REG);
	printf (&debug, "  RPC        = %04X\n", RPC_REG);

	/* Регистры банка 1 */
        BS_REG = 1;
        printf (&debug, "  CONFIG     = %04X\n", CONFIG_REG);
        printf (&debug, "  BASE       = %04X\n", BASE_REG);
        printf (&debug, "  ADDR0      = %04X\n", ADDR0_REG);
        printf (&debug, "  ADDR1      = %04X\n", ADDR1_REG);
        printf (&debug, "  ADDR2      = %04X\n", ADDR2_REG);
        printf (&debug, "  GP         = %04X\n", GP_REG);
        printf (&debug, "  CTL        = %04X\n", CTL_REG);

	/* Регистры банка 2 */
        BS_REG = 2;
	printf (&debug, "  MMU_CMD    = %04X\n", MMU_CMD_REG);
	printf (&debug, "  PN         = %02X\n", PN_REG);
	printf (&debug, "  AR         = %02X\n", AR_REG);
	printf (&debug, "  FIFO       = %04X\n", FIFO_REG);
	printf (&debug, "  PTR        = %04X\n", PTR_REG);
	printf (&debug, "  INT        = %02X\n", INT_REG);
	printf (&debug, "  IM         = %02X\n", IM_REG);

	/* Регистры банка 3 */
        BS_REG = 3;
        printf (&debug, "  MCASTL     = %08X\n", MCASTL_REG);
        printf (&debug, "  MCASTH     = %08X\n", MCASTH_REG);
        printf (&debug, "  MII        = %04X\n", MII_REG);
        printf (&debug, "  REV        = %04X\n", REV_REG);
        printf (&debug, "  ERCV       = %04X\n", ERCV_REG);
	printf (&debug, "  BSR        = %04X\n", BS_REG);
}

/*
 * Установка параметров теста.
 */
void eth_config ()
{
	int cmd;
again:
	printf (&debug, "\n*** Тест контроллера Ethernet ***\n");

	printf (&debug, "\n  1. Внешняя заглушка: %s",
		eth_extloop_installed ? "установлена" : "не установлена");
	printf (&debug, "\n  2. Циклическое чтение регистра BSR");
	printf (&debug, "\n  3. Циклическая запись-чтение регистра BSR");
	printf (&debug, "\n  4. Просмотр регистров контроллера 91с111");
	puts (&debug, "\n\n");
	for (;;) {
		/* Ввод команды. */
		cmd = get_command ();
		if (cmd == '\n' || cmd == '\r' || cmd == '\33')
			return;

		if (cmd == '1') {
			eth_extloop_installed = ! eth_extloop_installed;
			break;
		}
		if (cmd == '2') {
			bsr_read_test ();
			break;
		}
		if (cmd == '3') {
			bsr_write_test ();
			break;
		}
		if (cmd == '4') {
			show_regs ();
			break;
		}
	}
	goto again;
}
