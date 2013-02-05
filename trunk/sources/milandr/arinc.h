#ifndef __ARINC_H__
#define __ARINC_H__

//! Использовать бит паритета
#define ARINC_FLAG_PAR_EN 1

//! Паритет как дополнение до нечётности
#define ARINC_FLAG_PAR_ODD 2

//! Скорость передачи.
//! 0 - 100 кГц
//! 1 - 12,5 кГц
#define ARINC_FLAG_CLK_12_5kHz 4

//! Инициализация передатчика ARINC.
//! \param out_channel Номер передатчика (1..4)
//! \param flags Массив флагов для настройки передатчика.
//! Смотри значения макросов ARINC_FLAG_<...> выше. Можно
//! комбинировать с помощью битового ИЛИ.
//! \return 0, если инициализация выполнена без ошибок.
int arinc_init_tx(int out_channel, unsigned int flags);

//! Инициализация приёмника ARINC.
//! \param in_channel Номер приёмника (1..8)
//! \return 0, если инициализация выполнена без ошибок.
int arinc_init_rx(int in_channel);

//! Выполняет запись 32-битного слова ARINC-429 для указанного передатчика.
//! \param out_channel Номер передатчика (1..4)
//! \param msg Слово ARINC для передачи.
//! \return 0, если передача успешно выполнена.
int arinc_write(int out_channel, ARINC_msg_t msg);

//! Выполняет чтение 32-битного слова ARINC-429 для указанного приёмника.
//! \param in_channel Номер приёмника (1..8)
//! \param [out] msg Указатель, по которому будет записано слово ARINC.
//! \return 0, если приём успешно выполнен.
int arinc_read(int in_channel, ARINC_msg_t *msg);

#endif
