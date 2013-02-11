#include <runtime/lib.h>
#include <kernel/uos.h>
#include <stream/stream.h>
#include <kernel/internal.h>
#include <milandr/mil-std-1553_bc.h>

// Номер контроллера MIL-STD (0 или 1)
#define MY_MIL_STD_PORT 0
// Канал основной или резервный (0 или 1)
#define MY_MIL_STD_CHANNEL 0
// Минимальный период в циклограмме или продолжительность одного слота (в миллисекундах)
#define CYCLOGRAM_SLOT_TIME 200
// Количество слотов в циклограмме
#define CYCLOGRAM_SLOTS_COUNT 5

ARRAY (test_milstd_bc_stack, 1000);

// Буфер для приёма данных (по 32 слова для 32-х подадресов) из канала MIL-STD.
static unsigned short mil_std_rx_buffer[MIL_STD_DATA_WORDS_COUNT];

// Буфер для выдачи данных (по 32 слова для 32-х подадресов) в канал MIL-STD.
static unsigned short mil_std_tx_buffer[MIL_STD_DATA_WORDS_COUNT];

static void test_milstd_bc_main();

static cyclogram_slot_t cyclogram_data[CYCLOGRAM_SLOTS_COUNT];
static mil_std_bc_t mil_bc;

// debug
// Адрес ОУ
static const unsigned char rt_addr = 2;
// Подадрес для получения данных от ОУ (ОУ-КШ)
static const unsigned char rt_subaddr_from = 7;
// Подадрес для передачи данных в ОУ (КШ-ОУ)
static const unsigned char rt_subaddr_to = 8;

void uos_init (void)
{
    debug_printf("bc_init\n");

    cyclogram_data[0].addr_dest = rt_addr;
    cyclogram_data[0].subaddr_dest = rt_subaddr_to;
    cyclogram_data[0].addr_source = -1;
    cyclogram_data[0].subaddr_source = -1;
    cyclogram_data[0].words_count = 3;
    cyclogram_data[1].words_count = 0;
    cyclogram_data[2].addr_dest = -1;
    cyclogram_data[2].subaddr_dest = -1;
    cyclogram_data[2].addr_source = rt_addr;
    cyclogram_data[2].subaddr_source = rt_subaddr_from;
    cyclogram_data[2].words_count = 3;
    cyclogram_data[3].words_count = 0;
    cyclogram_data[4].words_count = 0;

    mil_std_1553_init_pins(MY_MIL_STD_PORT);
    mil_std_1553_bc_init(&mil_bc,
                         MY_MIL_STD_PORT,
                         MY_MIL_STD_CHANNEL,
                         cyclogram_data,
                         CYCLOGRAM_SLOT_TIME,
                         CYCLOGRAM_SLOTS_COUNT,
                         KHZ,
                         mil_std_rx_buffer,
                         mil_std_tx_buffer);

    task_create(test_milstd_bc_main, 0, "test_milstd_bc_main", 1, test_milstd_bc_stack, sizeof(test_milstd_bc_stack));
}

// example
static void test_milstd_bc_main()
{
/*
    debug_printf("send command\n");
    mil_bc.reg->CommandWord1 |=
            // Количество слов выдаваемых данных
            MIL_STD_COMWORD_WORDSCNT_CODE(CMD_UnlockSender) |
            // Подадрес приёмника
            MIL_STD_COMWORD_SUBADDR_MODE(0x1f) |
            // Адрес приёмника
            MIL_STD_COMWORD_ADDR(rt_addr);
    mil_bc.reg->CONTROL |= MIL_STD_CONTROL_BCSTART;
*/

    const int txIndex = MIL_STD_SUBADDR_WORD_INDEX(rt_subaddr_to);
    const int rxIndex = MIL_STD_SUBADDR_WORD_INDEX(rt_subaddr_from);

    mutex_lock(&mil_bc.lock);
    mil_std_tx_buffer[txIndex + 0] = 0x71;
    mil_std_tx_buffer[txIndex + 1] = 0x72;
    mil_std_tx_buffer[txIndex + 2] = 0x73;
    mutex_unlock(&mil_bc.lock);

    int i = 0;

    for (;;)
    {
        debug_printf("bc iter\n");
        int a;
        for (a=0; a<2; ++a)
        {
            mutex_lock(&mil_bc.lock);
            for (i=0; i<3; ++i)
                ++mil_std_tx_buffer[txIndex + i];
            mutex_unlock(&mil_bc.lock);

            debug_printf("\n");

            debug_printf("bc(%d%s): dataToBeSent  =", mil_bc.port, mil_bc.channel == 1 ? "B" : "A");
            for (i=0; i<3; ++i)
                debug_printf(" %x", mil_std_tx_buffer[txIndex + i]);
            debug_printf("\n");

            debug_printf("bc(%d%s): dataToBeRecvd =", mil_bc.port, mil_bc.channel == 1 ? "B" : "A");
            for (i=0; i<3; ++i)
                debug_printf(" %x", mil_std_rx_buffer[rxIndex + i]);
            debug_printf("\n");

            mdelay(5000);
        }

        int ch = (~mil_bc.channel) & 1;
        mil_std_1553_set_bc_channel(&mil_bc, ch);
    }
}
