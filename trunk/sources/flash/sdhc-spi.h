//
// Драйвер карт SDHC поверх SPI
//
// Драйвер реализует интерфейс флеш-памяти flashif_t. Поддерживаются
// Secure Digital-карты, объёма больше 2 Гб (SDHC). На SDXC драйвер
// не проверялся.
// Драйвер рассчитан только на архитектуру типа little-endian. Доработки
// для big-endian не должны быть существенными, но было возможности
// их проверить.
//

#ifndef __SDHC_SPI_H__
#define __SDHC_SPI_H__

#include <flash/flash-interface.h>
#include <spi/spi-master-interface.h>

#define SDHC_STATE_IDLE         0
#define SDHC_STATE_MULTIREAD    1
#define SDHC_STATE_MULTIWRITE   2

//
// Структура драйвера SDHC
//
struct _sdhc_spi_t
{
    flashif_t       flashif;        // Интерфейс флеш-памяти
    spimif_t       *spi;            // Указатель на драйвер SPI
    spi_message_t   msg;            // Сообщение SPI
    uint8_t         databuf[40];    // Буфер для данных SPI
    uint8_t         state;          // Внутреннее состояние драйвера
    uint32_t        next_page;      // Следующая страница (для чтения 
                                    // или записи)
};
typedef struct _sdhc_spi_t sdhc_spi_t;

//
// Инициализация драйвера.
// m    - драйвер флеш-памяти
// s    - драйвер SPI
// mode - режим драйвера SPI
//
void sd_spi_init(sdhc_spi_t *m, spimif_t *s, unsigned mode);

#endif
