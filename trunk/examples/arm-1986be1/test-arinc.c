#include <runtime/lib.h>
#include <runtime/cortex-m1/io-1986ve1.h>
#include <milandr/arinc.h>

#include <kernel/uos.h>  // debug
#include <timer/timer.h>  // debug

// Для подключения портов микроконтроллера к выводам интерфейсных микросхем необходимо
// установить положение конфигурационных PB0, PB1, PB2, PB3, PB4, PB5, PB6, PB7 в левое положение,
// PF13, PF14 в нижнее положение, PD13, PD14 замкнуть.
// Сопротивление нагрузки для передатчиков интерфейса должно быть не менее 600 Ом.
//
// Назначение выводов разъёма XP129:
//  1 - IN1+
//  2 - IN2+
//  3 - IN3+
//  4 - IN4+
//  5 - GND
//  6 - IN1–
//  7 - IN2–
//  8 - GND
//  9 - IN3–
// 10 - IN4–
// 11 - OUT2+
// 12 - OUT2–
// 13 - GND
// 14 - OUT1+
// 15 - OUT1–
//
// Для работы теста выбрать номера каналов передачи и приёма. Замкнуть соответствующие
// выводы разъёма. Для правильной работы каналы на передачу и приём должны иметь разные номера.

#define MY_IN_CHANNEL 0
#define MY_OUT_CHANNEL 0

unsigned int func_tx = -1;
unsigned int func_rx = -1;
unsigned int control1_tx = -1;
unsigned int control1_rx = -1;
unsigned int control2_rx = -1;

extern timer_t timer;

arinc_t atx, arx;

void test_arinc_main()
{
    debug_printf("func_tx = %x, control1_tx = %x\n", func_tx, control1_tx);
    debug_printf("func_rx = %x, control1_rx = %x, control2_rx = %x\n", func_rx, control1_rx, control2_rx);

    debug_printf("channel out = %d, in = %d\n", MY_OUT_CHANNEL, MY_IN_CHANNEL);
    debug_printf("ssm data sdi label\n");

    unsigned int counter = 0xee;
    ARINC_msg_t *const pCounter = (ARINC_msg_t *)&counter;
    pCounter->label = 0x30;
    pCounter->sdi = 1;
    pCounter->data = 0x1e;

    const int fifoCnt = 60;
    int i = 0;
    unsigned int data = 0;
    ARINC_msg_t *const pData = (ARINC_msg_t *)&data;

    int received = 0;

    unsigned long time0 = timer_milliseconds(&timer);

    for(;;)
    {
//        const ARINC_msg_t firstSentData = *pCounter;

        for (i=0; i<fifoCnt; ++i)
        {
            arinc_write(&atx, *pCounter);

            debug_printf("sent = %x = %x %x %x %x, status = %x\n", counter, pCounter->ssm, pCounter->data, pCounter->sdi, pCounter->label, ARM_ARINC429T->STATUS);
            ++counter;
        }

        for(;;)
        {
            if (arinc_read(&arx, pData))
            {
                ++received;
                debug_printf("received = %08x = %x %x %x %x, status1 = %x, status2 = %x\n", data, pData->ssm, pData->data, pData->sdi, pData->label, ARM_ARINC429R->STATUS1, ARM_ARINC429R->STATUS2);

                // debug
                if (timer_passed(&timer, time0, 1000))
                {
                    static int ddd = 0;
                    if (ddd == 0)
                        debug_printf("received frames cnt = %d                      -------------\n", received);
                    ddd = 1;
                }
            }
            else
            {
                if ((ARM_ARINC429R->STATUS1 & ARM_ARINC429R_STATUS1_ERR(MY_IN_CHANNEL)) > 0)
                {
                    // Ошибка приёма
                    debug_printf("rec error\n");
                }
                else
                {
                    debug_printf("no data received\n");
                }
                break;
            }
        }

        timer_delay(&timer, 1000);
//        mdelay(1000);
    }
}

ARRAY (test_arinc_stack, 1000);
#define MSEC_PER_TICK 100  // период между прерываниями

timer_t timer;

void uos_init (void)
{
    arinc_init(&atx, MY_OUT_CHANNEL, 0, 0, ARINC_FLAG_PAR_EN | ARINC_FLAG_PAR_ODD);

    arinc_init(&arx, MY_IN_CHANNEL, 0, 0, ARINC_FLAG_PAR_EN | ARINC_FLAG_PAR_ODD | ARINC_FLAG_RX);
        
    arinc_init_pins(&atx);
    arinc_init_pins(&arx);

    timer_init(&timer, KHZ, MSEC_PER_TICK);

    task_create(test_arinc_main, 0, "test_arinc_main", 1, test_arinc_stack, sizeof(test_arinc_stack));
}
