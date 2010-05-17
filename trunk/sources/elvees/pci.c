/*
 * Обмен данными с шиной PCI в режиме Master
 */
#include <runtime/lib.h>
#include <elvees/pci.h>
#include <elvees/mcb-01.h>

void pci_init ()
{
//	device_id = MCB_PCI_DEVICE_VENDOR_ID;		/* Идентификация устройства */
//	subsystem_id = MCB_PCI_SUBSYSTEM_VENDOR_ID;	/* Идентификация подсистемы */
//	class_revision = MCB_PCI_CLASS_REVISION;	/* Регистр кода */

	MCB_PCI_STATUS_COMMAND =			/* Состояние и управление */
		MCB_PCI_COMMAND_MASTER;			/* Режим задатчика */

	MCB_PCI_CSR_PCI =				/* Управление шиной PCI */
		MCB_PCI_CSR_PCI_WN (8);			/* Уровень FIFO записи в память */

	MCB_PCI_BAR = 0;				/* Базовый адрес 0 */
	MCB_PCI_LATENCY_TIMER = 0;			/* Таймер времени передачи (MLT) */
	MCB_PCI_INTERRUPT_LINE = 0;			/* Код прерывания */
	MCB_PCI_MASKR_PCI = 0;				/* Маскирование прерываний */

	MCB_PCI_CSR_MASTER = 0;				/* Состояние и управление обменом в режиме Master */
	MCB_PCI_IR_MASTER = 0;				/* Адрес памяти при обмене в режиме Master */
	MCB_PCI_AR_PCI = 0;				/* Идентификатор адресуемого устройства */
}

/*
 * Чтение/запись конфигурационых регистров PCI-устройства.
 * Параметр cmd задаёт тип операции: MCB_PCI_CSR_MASTER_CFGREAD
 * или MCB_PCI_CSR_MASTER_CFGWRITE.
 * Возвращает 0 в случае фатальной ощибки.
 */
int pci_cfg_transaction (unsigned cmd, unsigned local_addr,
	unsigned cfgtype, unsigned funreg, unsigned idsel)
{
	/* Перед запуском выполнения транзакции передачи данных
	 * в режиме Master необходимо убедиться в том, что в настоящий
	 * момент времени транзакция не выполняется: в регистре
	 * CSR_Master бит RUN = 0. */
	if (MCB_PCI_CSR_MASTER & MCB_PCI_CSR_MASTER_RUN)
		return 0;
retry:
	/* Затем необходимо записатьадрес слова данных в регистр IR_Master. */
	MCB_PCI_IR_MASTER = local_addr;

	/* При выполнении конфигурационных операций разряды
	 * AR_PCI[1:0] определяют тип обмена (Type0 или Type1),
	 * а унитарный код в разрядах AR_PCI[31:11] указывает IDSEL
	 * адресуемого устройства. Разряды AR_PCI[10:2] должны
	 * содержать номер функции и регистра. */
	MCB_PCI_AR_PCI = cfgtype | funreg << 2 | idsel << 11;

	/* - команду CMD, число слов данных WC и бит RUN=1 в регистр CSR_Master. */
	MCB_PCI_CSR_MASTER = cmd |
		MCB_PCI_CSR_MASTER_WC (1) |		/* Счетчик слов DMA обмена */
		MCB_PCI_CSR_MASTER_RUN;			/* Состояние работы канала DMA */

	/* После завершения выполнения транзакции:
	 * - в регистре CSR_Master: RUN=0, DONE=1. */
	while (MCB_PCI_CSR_MASTER & MCB_PCI_CSR_MASTER_RUN)
		continue;

	/* Поле WC может быть или равно 0 или нет.
	 * В регистре IR_Master содержится увеличенный на 4 адрес
	 * последнего переданного  слова данных. */

	/* После завершения выполнения транзакции необходимо проверить
	 * состояние битов регистра CSR_PCI. */
	unsigned csr_pci = MCB_PCI_CSR_PCI;
	if (csr_pci & (MCB_PCI_CSR_PCI_NOTRDY |
	    MCB_PCI_CSR_PCI_NOGNT | MCB_PCI_CSR_PCI_TARGET_ABORT |
	    MCB_PCI_CSR_PCI_MASTER_ABORT)) {
		/* Если хотя бы один бит No Trdy, No Gnt, Target Abort,
		 * Master Abort не равен нулю, то этот обмен данными
		 * невозможно выполнить. */
		 return 0;
	}
	if (! (csr_pci & (MCB_PCI_CSR_PCI_MLTOVER |
	    MCB_PCI_CSR_PCI_DISCONNECT | MCB_PCI_CSR_PCI_RETRY))) {
		/* Если все биты Mlt Over, Disconnect, Retry равны нулю,
		 * то передача данных завершена нормально. */
		 return 1;
	}
	if (csr_pci & MCB_PCI_CSR_PCI_RETRY) {
		/* Если Retry=1, то необходимо повторить транзакцию,
		 * восстановив первоначальное содержимое регистров
		 * IR_Master, AR_PCI, CSR_Master. */
		goto retry;
	}
	return 0;
}

/*
 * Чтение/запись памяти или i/o PCI-устройства.
 * Параметр cmd задаёт тип операции:
 *	MCB_PCI_CSR_MASTER_IOREAD
 *	MCB_PCI_CSR_MASTER_IOWRITE
 *	MCB_PCI_CSR_MASTER_MEMREAD
 *	MCB_PCI_CSR_MASTER_MEMWRITE
 * Возвращает 0 в случае фатальной ощибки.
 */
int pci_data_transaction (unsigned cmd, unsigned local_addr,
	unsigned pci_addr, unsigned nwords)
{
	/* Перед запуском выполнения транзакции передачи данных
	 * в режиме Master необходимо убедиться в том, что в настоящий
	 * момент времени транзакция не выполняется: в регистре
	 * CSR_Master бит RUN = 0. */
	if (MCB_PCI_CSR_MASTER & MCB_PCI_CSR_MASTER_RUN)
		return 0;
retry:
	/* Затем необходимо записать:
	 * - адрес первого слова данных в регистр IR_Master; */
	MCB_PCI_IR_MASTER = local_addr;

	/* - начальный адрес устройства на шине PCI в регистр AR_PCI; */
	MCB_PCI_AR_PCI = pci_addr;

	/* - команду CMD, число слов данных WC и бит RUN=1 в регистр CSR_Master. */
	MCB_PCI_CSR_MASTER = cmd |
		MCB_PCI_CSR_MASTER_WC (nwords) |	/* Счетчик слов DMA обмена */
		MCB_PCI_CSR_MASTER_RUN;			/* Состояние работы канала DMA */
wait:
	/* После завершения выполнения транзакции:
	 * - в регистре CSR_Master: RUN=0, DONE=1. */
	while (MCB_PCI_CSR_MASTER & MCB_PCI_CSR_MASTER_RUN)
		continue;

	/* Поле WC может быть или равно 0 или нет.
	 * В регистре IR_Master содержится увеличенный на 4 адрес
	 * последнего переданного  слова данных. */

	/* После завершения выполнения транзакции необходимо проверить
	 * состояние битов регистра CSR_PCI. */
	unsigned csr_pci = MCB_PCI_CSR_PCI;
	if (csr_pci & (MCB_PCI_CSR_PCI_NOTRDY |
	    MCB_PCI_CSR_PCI_NOGNT | MCB_PCI_CSR_PCI_TARGET_ABORT |
	    MCB_PCI_CSR_PCI_MASTER_ABORT)) {
		/* Если хотя бы один бит No Trdy, No Gnt, Target Abort,
		 * Master Abort не равен нулю, то этот обмен данными
		 * невозможно выполнить. */
		 return 0;
	}
	if (! (csr_pci & (MCB_PCI_CSR_PCI_MLTOVER |
	    MCB_PCI_CSR_PCI_DISCONNECT | MCB_PCI_CSR_PCI_RETRY))) {
		/* Если все биты Mlt Over, Disconnect, Retry равны нулю,
		 * то передача данных завершена нормально. */
		 return 1;
	}
	if (csr_pci & MCB_PCI_CSR_PCI_RETRY) {
		/* Если Retry=1, то необходимо повторить транзакцию,
		 * восстановив первоначальное содержимое регистров
		 * IR_Master, AR_PCI, CSR_Master. */
		goto retry;
	}
	/* Случай Retry=0, и один из битов Disconnect и Mlt Over равен 1.
	 * Если выполняется транзакция записи данных в шину PCI, то
	 * необходимо повторить транзакцию, восстановив исходное содержимое
	 * регистров IR_Master, AR_PCI, CSR_Master. */
	if (cmd == MCB_PCI_CSR_MASTER_IOWRITE ||
	    cmd == MCB_PCI_CSR_MASTER_MEMWRITE)
		goto retry;

	/* Если выполняется транзакция чтения данных из шины PCI, то можно
	 * продолжить эту транзакцию.
	 * 1) Содержимое AR_PCI инкрементировать на величину 4*(WCbegin - WCend),
	 * где WCbegin – содержимое поля WC регистра CSR_Master перед запуском этой
	 * операции, а WCend – содержимое поля WC регистра CSR_Master после
	 * завершения выполнения транзакции. */
	unsigned residual = MCB_PCI_CSR_MASTER >> 16;
	MCB_PCI_AR_PCI += (nwords - residual) * 4;

	/* 2) Запустить выполнение транзакции чтения данных. Для этого
	 * необходимо в регистре CSR_Master установить только бит RUN=1,
	 * сохранив содержимое полей WC и CMD. */
	MCB_PCI_CSR_MASTER |= MCB_PCI_CSR_MASTER_RUN;
	goto wait;
}

/*
 * Чтение конфигурационых регистров PCI-устройства.
 * Возвращает 0 в случае фатальной ощибки.
 */
int pci_cfg_read (unsigned dev, unsigned function, unsigned reg,
	unsigned *result)
{
	/* Конфигурационная транзакция Type 0. */
	unsigned local_addr = 0;
	if (! pci_cfg_transaction (MCB_PCI_CSR_MASTER_CFGREAD,
	    local_addr, 0, function << 6 | reg, 1 << dev))
		return 0;
	*result = MCB_REGISTER (MCB_RAM_BASE, local_addr);
	return 1;
}

/*
 * Запись конфигурационых регистров PCI-устройства.
 * Возвращает 0 в случае фатальной ощибки.
 */
int pci_cfg_write (unsigned dev, unsigned function, unsigned reg,
	unsigned value)
{
	/* Конфигурационная транзакция Type 0. */
	unsigned local_addr = 0;
	MCB_REGISTER (MCB_RAM_BASE, local_addr) = value;
	if (! pci_cfg_transaction (MCB_PCI_CSR_MASTER_CFGWRITE,
	    local_addr, 0, function << 6 | reg, 1 << dev))
		return 0;
	return 1;
}

/*
 * Чтение 32-битного слова из i/o-пространства PCI-устройства.
 * Возвращает 0 в случае фатальной ощибки.
 */
int pci_io_read (unsigned addr, unsigned *result)
{
	unsigned local_addr = 0;
	if (! pci_data_transaction (MCB_PCI_CSR_MASTER_IOREAD,
	    local_addr, addr, 1))
		return 0;
	*result = MCB_REGISTER (MCB_RAM_BASE, local_addr);
	return 1;
}

/*
 * Запись 32-битного слова в i/o-пространство PCI-устройства.
 * Возвращает 0 в случае фатальной ощибки.
 */
int pci_io_write (unsigned addr, unsigned value)
{
	unsigned local_addr = 0;
	MCB_REGISTER (MCB_RAM_BASE, local_addr) = value;
	if (! pci_data_transaction (MCB_PCI_CSR_MASTER_IOWRITE,
	    local_addr, addr, 1))
		return 0;
	return 1;
}

/*
 * Чтение массива 32-битных слов из памяти PCI-устройства.
 * Возвращает 0 в случае фатальной ощибки.
 */
int pci_mem_read (unsigned addr, unsigned *data, unsigned nwords)
{
	unsigned local_addr = 0;
	if (! pci_data_transaction (MCB_PCI_CSR_MASTER_MEMREAD,
	    local_addr, addr, nwords))
		return 0;
	memcpy (data, (unsigned*) (MCB_RAM_BASE + local_addr), nwords*4);
	return 1;
}

/*
 * Запись массива 32-битных слов в память PCI-устройства.
 * Возвращает 0 в случае фатальной ощибки.
 */
int pci_mem_write (unsigned addr, unsigned *data, unsigned nwords)
{
	unsigned local_addr = 0;
	memcpy ((unsigned*) (MCB_RAM_BASE + local_addr), data, nwords*4);
	if (! pci_data_transaction (MCB_PCI_CSR_MASTER_MEMWRITE,
	    local_addr, addr, nwords))
		return 0;
	return 1;
}
