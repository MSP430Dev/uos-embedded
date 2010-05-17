/*
 * Обмен данными с шиной PCI в режиме Master
 */
void pci_init (void);

/*
 * Чтение конфигурационых регистров PCI-устройства.
 * Возвращает 0 в случае фатальной ощибки.
 * Параметры:
 *	dev - номер устройства на шине PCI (от 0 до 20)
 *	function - номер функции внутри устройства (от 0 до 7)
 *	reg - номер конфигурационного регистра (от 0 до 63)
 * Результат помещается по адресу result.
 */
int pci_cfg_read (unsigned dev, unsigned function, unsigned reg,
	unsigned *result);

/*
 * Запись конфигурационых регистров PCI-устройства.
 * Возвращает 0 в случае фатальной ощибки.
 * Параметры:
 *	dev - номер устройства на шине PCI (от 0 до 20)
 *	function - номер функции внутри устройства (от 0 до 7)
 *	reg - номер конфигурационного регистра (от 0 до 63)
 */
int pci_cfg_write (unsigned dev, unsigned function, unsigned reg,
	unsigned value);

/*
 * Чтение 32-битного слова из i/o-пространства PCI-устройства.
 * Возвращает 0 в случае фатальной ощибки.
 * Результат помещается по адресу result.
 */
int pci_io_read (unsigned addr, unsigned *result);

/*
 * Запись 32-битного слова в i/o-пространство PCI-устройства.
 * Возвращает 0 в случае фатальной ощибки.
 */
int pci_io_write (unsigned addr, unsigned value);

/*
 * Чтение массива 32-битных слов из памяти PCI-устройства.
 * Возвращает 0 в случае фатальной ощибки.
 */
int pci_mem_read (unsigned addr, unsigned *data, unsigned nwords);

/*
 * Запись массива 32-битных слов в память PCI-устройства.
 * Возвращает 0 в случае фатальной ощибки.
 */
int pci_mem_write (unsigned addr, unsigned *data, unsigned nwords);
