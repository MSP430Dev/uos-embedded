#ifndef __MIL_STD_1553_RT_H__
#define __MIL_STD_1553_RT_H__

#include <kernel/uos.h>
#include "mil-std-1553_setup.h"

struct MIL_STD_1553B_t;

//! Значения регистра MSG в режиме ОУ и М.
typedef enum
{
    // Команды обмена данными
    //! Команда приёма КШ-ОУ, не групповая (формат сообщения 1)
    MSG_DATARECV__BC_RT__SINGLE = 1,
    //! Команда приёма  КШ-ОУ, групповая (формат сообщения 7)
    MSG_DATARECV__BC_RT__GROUP = 0x80,
    //! Команда приёма ОУ-ОУ, не групповая (формат сообщения 3)
    MSG_DATARECV__RT_RT__SINGLE = 4,
    //! Команда приёма ОУ-ОУ, групповая (формат сообщения 8)
    MSG_DATARECV__RT_RT__GROUP = 0x100,
    //! Команда передачи ОУ-КШ (формат сообщения 2)
    MSG_DATASEND__RT_BC__SINGLE = 0x402,
    //! Команда передачи ОУ-ОУ, не групповая (формат сообщения 3)
    MSG_DATASEND__RT_RT__SINGLE = 0x1008,
    //! Команда передачи ОУ-ОУ, групповая (формат сообщения 8)
    MSG_DATASEND__RT_RT__GROUP = 0x200,
    // Команды управления
    //! Код 0-15 K=1 нет данных, не групповая (формат сообщения 4)
    MSG_CMD_WITH_NODATA__0_0xf__SINGLE = 0x410,
    //! Код 0-15 K=1 нет данных, групповая (формат сообщения 9)
    MSG_CMD_WITH_NODATA__0_0xf__GROUP = 0x400,
    //! Код 16-31 K=1 с данными, не групповая (формат сообщения 5)
    MSG_CMD_WITH_NODATA__0x10_0x1f__DATAWORD_EXPECTED__SINGLE = 0x2420,
    //! Код 16-31 К=0 с данными, не групповая (формат сообщения 6)
    MSG_CMD_WITH_DATA__0x10_0x1f__SINGLE = 0x40,
    //! Код 16-31 К=0 с данными, групповая (формат сообщения 10)
    MSG_CMD_WITH_DATA__0x10_0x1f__GROUP = 0x800
} MSG_COMMANDS;

//! Data structure for MIL-STD RT.
typedef struct
{
    //! Номер контроллера MIL-STD (0, 1)
    unsigned char port;
    //! Собственный адрес устройства
    unsigned char addr_self;
    //! Указатель на структуру с регистрами управления контроллера MIL-STD-1553 (канал 1 или 2)
    MIL_STD_1553B_t *reg;
    //! Мьютекс для работы с прерываниями
    mutex_t lock;
    //! Буфер для приёма данных из канала
    unsigned short *rx_buf;
    //! Буфер для выдачи данных в канал
    unsigned short *tx_buf;
} mil_std_rt_t;

//! Инициализация контроллера интерфейса по ГОСТ Р52070-2003 (MIL-STD-1553)
//! для микроконтроллера Cortex-M1 в режиме оконечного устройства (ОУ).
//! \param rt Структура с описанием ОУ
//! \param port Номер контроллера (0 или 1)
//! \param addr_self Собственный адрес ОУ
//! \param rx_buf Буфер для приёма данных из канала
//! \param tx_buf Буфер для выдачи данных в канал
void mil_std_1553_rt_init(mil_std_rt_t *rt,
                          int port,
                          int addr_self,
                          void *rx_buf,
                          void *tx_buf);

#endif
