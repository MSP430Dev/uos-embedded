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

ARRAY (test_arinc_stack, 1000);
#define MSEC_PER_TICK 100  // период между прерываниями

timer_t timer;

arinc_t atx, arx;
unsigned atxbuf[256];
unsigned arxbuf[256];


void test_arinc_main()
{
    debug_printf("channel out = %d, in = %d\n", MY_OUT_CHANNEL, MY_IN_CHANNEL);
    debug_printf("ssm data sdi label\n");

    unsigned short tcnt = 0xee;
    ARINC_msg_t * pmsg = (ARINC_msg_t *)atxbuf;
    int i;
    unsigned char nb_labels = sizeof(atxbuf)/sizeof(atxbuf[0]);
	unsigned cycle_cnt = 0;


	nb_labels = 150;
    
    for(;;)
    {
        // Читаем массив принятых меток и печатаем их
        arinc_poll(&arx);
        for (i = 0; i < sizeof(arxbuf)/sizeof(arxbuf[0]); ++i) {
            if (i % 8 == 0)
                debug_printf ("\n");
            debug_printf("%08X ", arxbuf[i]);
        }
        debug_printf("\n\n");
		memset(arxbuf, 0, sizeof(arxbuf));
        
        // Заполняем массив и передаём его
        pmsg = (ARINC_msg_t *)atxbuf;
        for (i = 0; i < nb_labels; ++i) {
			if (i & 1)
				pmsg->label = nb_labels - i;
			else
				pmsg->label = i;
            pmsg->data = tcnt;
            pmsg++;
        }
		*((unsigned *) pmsg) = 0;
        tcnt++;
        arinc_poll(&atx);

        timer_delay(&timer, 1000);
		
		cycle_cnt++;
		if (cycle_cnt % 10 == 0)
			nb_labels += 50;
		debug_printf("nb_labels: %d\n", nb_labels);
    }
}

void uos_init (void)
{
    arinc_init(&atx, MY_OUT_CHANNEL, atxbuf, sizeof(atxbuf), 
        ARINC_FLAG_PAR_EN | ARINC_FLAG_PAR_ODD);

    arinc_init(&arx, MY_IN_CHANNEL, arxbuf, sizeof(arxbuf), 
        ARINC_FLAG_PAR_EN | ARINC_FLAG_PAR_ODD | ARINC_FLAG_RX);
        
    arinc_init_pins(&atx);
    arinc_init_pins(&arx);

    timer_init(&timer, KHZ, MSEC_PER_TICK);

    task_create(test_arinc_main, 0, "test_arinc_main", 1, test_arinc_stack, sizeof(test_arinc_stack));
}
