#include <runtime/lib.h>
#include <kernel/uos.h>
#include <stream/stream.h>
#include <kernel/internal.h>
#include <milandr/mil-std-1553_rt.h>

// Номер контроллера MIL-STD (0 или 1)
#define MY_MIL_STD_CHANNEL 0

// Собственный адрес ОУ
#define MIL_STD_SELF 2

ARRAY (test_milstd_rt_stack, 1000);

// Буфер для приёма данных (по 32 слова для 32-х подадресов) из канала MIL-STD.
static unsigned short mil_std_rx_buffer[MIL_STD_DATA_WORDS_COUNT];

// Буфер для выдачи данных (по 32 слова для 32-х подадресов) в канал MIL-STD.
static unsigned short mil_std_tx_buffer[MIL_STD_DATA_WORDS_COUNT];

static void test_milstd_rt_main();

static mil_std_rt_t mil_rt;

void uos_init (void)
{
    if (mil_std_1553_rt_init(&mil_rt, MY_MIL_STD_CHANNEL, MIL_STD_SELF, mil_std_rx_buffer, mil_std_tx_buffer) == 0)
    {
        task_create(test_milstd_rt_main, 0, "test_milstd_rt_main", 1, test_milstd_rt_stack, sizeof(test_milstd_rt_stack));
    }
    else
        debug_printf("mil-std (rt mode) init error\n");
}

// example
static void test_milstd_rt_main()
{
    // Подадрес для получения данных от КШ (КШ-ОУ)
    const unsigned char bc_subaddr_from = 8;
    // Подадрес для передачи данных в КШ (ОУ-КШ)
    const unsigned char bc_subaddr_to = 7;

    const int txIndex = MIL_STD_SUBADDR_WORD_INDEX(bc_subaddr_to);
    const int rxIndex = MIL_STD_SUBADDR_WORD_INDEX(bc_subaddr_from);

    mutex_lock(&mil_rt.lock);
    mil_std_tx_buffer[txIndex+0] = 0x30;
    mil_std_tx_buffer[txIndex+1] = 0x31;
    mil_std_tx_buffer[txIndex+2] = 0x32;
    mutex_unlock(&mil_rt.lock);

    int i=0;

    while (1)
    {
        mdelay(5000);

        mutex_lock(&mil_rt.lock);
        for (i=0; i<3; ++i)
            ++mil_std_tx_buffer[txIndex + i];
        mutex_unlock(&mil_rt.lock);

        debug_printf("\n");

        debug_printf("rt: dataToBeSent  =");
        for (i=0; i<3; ++i)
            debug_printf(" %x", mil_std_tx_buffer[txIndex + i]);
        debug_printf("\n");

        debug_printf("rt: dataToBeRecvd =");
        for (i=0; i<3; ++i)
            debug_printf(" %x", mil_std_rx_buffer[rxIndex + i]);
        debug_printf("\n");
    }
}
