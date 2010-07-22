/*
 * Register definitions for Milandr 1986BE9x.
 */

/*
 * Internal SRAM address and size.
 */
#define ARM_SRAM_BASE		0x20000000
#define ARM_SRAM_SIZE		0x00004000

/*
 * Base address of special registers.
 */
#define ARM_R(x)		(*(volatile unsigned*) (0x40000000 | (x)))

/*------------------------------------------------------
 * Описание регистров контроллера Flash памяти программ.
 */
#define ARM_EEPROM_CMD		ARM_R(0x18000)	/* Управление Flash-памятью */
#define ARM_EEPROM_ADR		ARM_R(0x18004)	/* Адрес (словный) */
#define ARM_EEPROM_DI		ARM_R(0x18008)	/* Данные для записи */
#define ARM_EEPROM_DO		ARM_R(0x1800C)	/* Считанные данные */
#define ARM_EEPROM_KEY		ARM_R(0x18010)	/* Ключ */

/*
 * Регистр EEPROM_CMD
 */
#define ARM_EEPROM_CMD_CON	0x00000001
				/*
				 * Переключение контроллера памяти EEPROM на
				 * регистровое управление. Не может производиться
				 * при исполнении программы из области EERPOM.
				 * 0 – управление EERPOM от ядра, рабочий режим
				 * 1 – управление от регистров, режим программирования
				 */

#define ARM_EEPROM_CMD_WR	0x00000002
				/*
				 * Запись в память EERPOM (в режиме программирования)
				 * 0 – нет записи
				 * 1 – есть запись
				 */

#define ARM_EEPROM_CMD_RD	0x00000004
				/*
				 * Чтение из память EERPOM (в режиме программирования)
				 * 0 – нет чтения
				 * 1 – есть чтение
				 */

#define ARM_EEPROM_CMD_DELAY_MASK	0x00000038
				/*
				 * Задержка памяти программ при чтении
				 */
#define ARM_EEPROM_CMD_DELAY_0	0x00000000	/* 0 тактов - до 25 МГц */
#define ARM_EEPROM_CMD_DELAY_1	0x00000008      /* 1 такт - до 50 МГц */
#define ARM_EEPROM_CMD_DELAY_2	0x00000010      /* 2 такта - до 75 МГц */
#define ARM_EEPROM_CMD_DELAY_3	0x00000018      /* 3 такта - до 100 МГц */
#define ARM_EEPROM_CMD_DELAY_4	0x00000020      /* 4 такта - до 125 МГц */
#define ARM_EEPROM_CMD_DELAY_5	0x00000028      /* 5 тактов - до 150 МГц */
#define ARM_EEPROM_CMD_DELAY_6	0x00000030      /* 6 тактов - до 175 МГц */
#define ARM_EEPROM_CMD_DELAY_7	0x00000038      /* 7 тактов - до 200 МГц */

#define ARM_EEPROM_CMD_XE 	0x00000040
				/*
				 * Выдача адреса ADR[16:9]
				 * 0 – не разрешено
				 * 1 - разрешено
				 */

#define ARM_EEPROM_CMD_YE 	0x00000080
				/*
				 * Выдача адреса ADR[8:2]
				 * 0 – не разрешено
				 * 1 – разрешено
				 */

#define ARM_EEPROM_CMD_SE 	0x00000100
				/*
				 * Усилитель считывания
				 * 0 – не включен
				 * 1 – включен
				 */

#define ARM_EEPROM_CMD_IFREN 	0x00000200
				/*
				 * Работа с блоком информации
				 * 0 – основная память
				 * 1 – информационный блок
				 */

#define ARM_EEPROM_CMD_ERASE 	0x00000400
				/*
				 * Стереть строку с адресом ADR[16:9].
				 * ADR[8:0] значения не имеет.
				 * 0 – нет стирания
				 * 1 – стирание
				 */

#define ARM_EEPROM_CMD_MAS1 	0x00000800
				/*
				 * Стереть весь блок, при ERASE=1
				 * 0 – нет стирания
				 * 1 – стирание
				 */

#define ARM_EEPROM_CMD_PROG 	0x00001000
				/*
				 * Записать данные по ADR[16:2] из регистра EERPOM_DI
				 * 0 – нет записи
				 * 1 – есть запись
				 */

#define ARM_EEPROM_CMD_NVSTR	0x00002000
				/*
				 * Операции записи или стирания
				 * 0 – при чтении
				 * 1 - при записи или стирании
				 */

/* End of Milandr 1986BE9x register definitions.
 *----------------------------------------------*/
