#ifndef __MIL_STD_1553_BC_H__
#define __MIL_STD_1553_BC_H__

#include <kernel/uos.h>

typedef enum
{
    CMD_TakeInterfaceControl = 0,  //!< Принять управление интерфейсом
    CMD_Synchronize = 1,  //!< Синхронизация
    CMD_SendAnswerWord = 2,  //!< Передать ОС
    CMD_SelfCheck = 3,  //!< Начать самоконтроль ОУ
    CMD_LockSender = 4,  //!< Блокировать передатчик
    CMD_UnlockSender = 5,  //!< Разблокировать передатчик
    CMD_LockInvalidRtFlag = 6,  //!< Блокировать признак неисправности ОУ
    CMD_UnlockInvalidRtFlag = 7,  //!< Разблокировать признак неисправности ОУ
    CMD_SetRtInitialState = 8,  //!< Установить ОУ в исходное состояние
    CMD_SendVectorWord = 0x10,  //!< Передать векторное слово
    CMD_SynchronizeWithDataWord = 0x11,  //!< Синхронизация (с СД)
    CMD_SendLastCommand = 0x12,  //!< Передать последнюю команду
    CMD_SendRtInternalCheckWord = 0x13,  //!< Передать слово ВСК (встроенная система контроля) ОУ
    CMD_LockSenderN = 0x14,  //!< Блокировать i-й передатчик
    CMD_UnlockSenderN = 0x15  //!< Разблокировать i-й передатчик
} BC_COMMAND_CODES;

//! Структура для определения одного слота в циклограмме.
//! Адрес приёмника может принимать значения 0..31; значение 31 используется
//! для передачи группового сообщения.
//! Подадрес приёмника может принимать значения 1..30; значения 0 или 31 используются
//! для передачи команды управления.
//! Адрес и подадрес для КШ указывается -1.
//! Если количество слов данных для передачи равно нулю,
//! считаем, что передачи данных нет, указанные адреса в этом случае
//! не имеют значения.
typedef struct
{
    //! Адрес источника данных.
    unsigned char addr_source;
    //! Подадрес источника данных.
    unsigned char subaddr_source;
    //! Адрес приёмника данных.
    unsigned char addr_dest;
    //! Подадрес приёмника данных.
    unsigned char subaddr_dest;
    //! Количество слов данных.
    unsigned char words_count;
} cyclogram_slot_t;

struct MIL_STD_1553B_t;

//! Data structure for MIL-STD BC
typedef struct
{
    //! port number (0, 1)
    unsigned char port;
    //! Количество слотов в циклограмме
    unsigned char slots_count;
    //! Индекс текущего слота (0..0xfe)
    unsigned char curr_slot;
    //! Индекс слота, для которого ожидается ответ от ОУ (передача данных ОУ-КШ).
    unsigned char rt_bc_slot;
    //! Указатель на структуру с регистрами управления контроллера MIL-STD-1553 (канал 1 или 2)
    MIL_STD_1553B_t *reg;
    //! Циклограмма для КШ
    const cyclogram_slot_t *cyclogram;
    //! Мьютекс для работы с прерываниями
    mutex_t lock;  // (?) Два мьютекса: для буферов rx и tx?
    //! Буфер для приёма данных из канала
    unsigned short *rx_buf;
    //! Буфер для выдачи данных в канал
    unsigned short *tx_buf;
} mil_std_bc_t;

//! Инициализация контроллера MIL-STD-1553B в режиме КШ.
//! \param bc Структура с описанием КШ
//! \param port Номер контроллера (0 или 1)
//! \param cyclogram Циклограмма как массив структур
//! \param slot_time Длительность минимального слота циклограммы в миллисекундах
//! \param slots_count Количество слотов в циклограмме (количество элементов в массиве)
//! \param cpu_freq Частота работы микроконтроллера в КГц
//! \param rx_buf Буфер для приёма данных из канала
//! \param tx_buf Буфер для выдачи данных в канал
int mil_std_1553_bc_init(mil_std_bc_t *bc,
                         int port,
                         const cyclogram_slot_t *cyclogram,
                         int slot_time,
                         int slots_count,
                         int cpu_freq,
                         unsigned short *rx_buf,
                         unsigned short *tx_buf);

#endif
