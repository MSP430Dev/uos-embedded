#ifdef ARM_1986BE1

#include <runtime/lib.h>
#include <kernel/uos.h>
#include <kernel/internal.h>
#include <runtime/cortex-m1/io-1986ve1.h>
#include <milandr/mil-std-1553_setup.h>
#include <milandr/mil-std-1553_bc.h>

#define BC_DEBUG 0

//
static mutex_t timerMutex;
// (?) Нужен ли этот мьютекс или можно обойтись bc->lock?
// (?) Нужно ли вызывать функцию mutex_init()?
static mutex_t milstdMutex;

static const int timer_irq = TIMER1_IRQn;

static bool_t mil_std_1553_bc_handler(void *arg)
{
    mil_std_bc_t *bc = (mil_std_bc_t *)arg;

    const unsigned int locStatus = bc->reg->STATUS;

    const int locIrqNum = bc->port == 0 ? MIL_STD_1553B1_IRQn : MIL_STD_1553B2_IRQn;

#if BC_DEBUG
    debug_printf("handler, STATUS=%x, StatWrd1=%x, CONTROL=%x, ERROR=%x\n",
                 locStatus,
                 bc->reg->StatusWord1,
                 bc->reg->CONTROL,
                 bc->reg->ERROR);
#endif

    if ((locStatus & MIL_STD_STATUS_ERR) != 0)
    {
#if BC_DEBUG
        debug_printf("error received\n");
#endif

        // (?)
        mil_std_1553_setup(bc->port, MIL_STD_MODE_BC, 0);

        // Разрешить прерывания:
        // при приёме достоверного слова,
        // при возникновении ошибки в сообщении.
        bc->reg->INTEN = MIL_STD_INTEN_RFLAGNIE | MIL_STD_INTEN_ERRIE;
    }
    else if ((locStatus & MIL_STD_STATUS_RFLAGN) != 0)
    {
        if (bc->rt_bc_slot < bc->slots_count)
        {
            const cyclogram_slot_t *const expectedSlot = &bc->cyclogram[bc->rt_bc_slot];

            const int index = MIL_STD_SUBADDR_WORD_INDEX(expectedSlot->subaddr_source);
            int i = 0;

            mutex_lock(&bc->lock);
            for (i=0; i<expectedSlot->words_count; ++i)
                bc->rx_buf[index + i] = bc->reg->DATA[index + i];
            mutex_unlock(&bc->lock);

#if BC_DEBUG
            debug_printf("handler, data received =");
            for (i=0; i<expectedSlot->words_count; ++i)
                debug_printf(" %x", bc->rx_buf[index + i]);
            debug_printf("\n");
#endif

            bc->rt_bc_slot = 0xff;
        }
    }

    arch_intr_allow(locIrqNum);

    return 0;
}

static bool_t mil_std_1553_timer_handler(void *arg)
{
#if BC_DEBUG
    debug_printf("timer handler, ");
#endif

    mil_std_bc_t *const bc = (mil_std_bc_t *)arg;

    // (!) Здесь не вызывается повторное разрешение прерывания
    if (bc->cyclogram == 0 || bc->slots_count == 0)
        return 0;

    const cyclogram_slot_t *const currSlot = &bc->cyclogram[bc->curr_slot];

    bc->rt_bc_slot = 0xff;

#if BC_DEBUG
    debug_printf("slot #%d: ", bc->curr_slot);
#endif

    if (currSlot->words_count > 0)
    {
        // Режим передачи КШ-ОУ
        if (currSlot->addr_source == 0xff || currSlot->subaddr_source == 0xff)
        {
            bc->reg->CommandWord1 |=
                    // Количество слов выдаваемых данных
                    MIL_STD_COMWORD_WORDSCNT_CODE(currSlot->words_count) |
                    // Подадрес приёмника
                    MIL_STD_COMWORD_SUBADDR_MODE(currSlot->subaddr_dest) |
                    // Адрес приёмника
                    MIL_STD_COMWORD_ADDR(currSlot->addr_dest);

            int i = 0;
            int index = MIL_STD_SUBADDR_WORD_INDEX(currSlot->subaddr_dest);

            // Копировать из tx буфера данные в буфер контроллера MIL-STD
            mutex_lock(&bc->lock);
            for (i=0; i<currSlot->words_count; ++i)
                bc->reg->DATA[index + i] = bc->tx_buf[index + i];
            mutex_unlock(&bc->lock);

#if BC_DEBUG
            debug_printf("bc->rt, slot = %x, %x, %x, %x, %x, dataToSend =",
                         currSlot->addr_source,
                         currSlot->subaddr_source,
                         currSlot->addr_dest,
                         currSlot->subaddr_dest,
                         currSlot->words_count);
            for (i=0; i<currSlot->words_count; ++i)
                debug_printf(" %x", bc->tx_buf[index + i]);
            debug_printf("\n");
#endif
        }
        // Режим передачи ОУ-КШ
        else if (currSlot->addr_dest == 0xff || currSlot->subaddr_dest == 0xff)
        {
            // Количество слов данных
            bc->reg->CommandWord1 |=
                    // Количество слов принимаемых данных
                    MIL_STD_COMWORD_WORDSCNT_CODE(currSlot->words_count) |
                    // Подадрес источника
                    MIL_STD_COMWORD_SUBADDR_MODE(currSlot->subaddr_source) |
                    // Направление передачи: ОУ-КШ
                    MIL_STD_COMWORD_RT_BC |
                    // Адрес источника
                    MIL_STD_COMWORD_ADDR(currSlot->addr_source);
            bc->rt_bc_slot = bc->curr_slot;

#if BC_DEBUG
            debug_printf("rt->bc, slot = %x, %x, %x, %x, %x\n",
                         currSlot->addr_source,
                         currSlot->subaddr_source,
                         currSlot->addr_dest,
                         currSlot->subaddr_dest,
                         currSlot->words_count);
#endif
        }
        // Режим передачи ОУ-ОУ
        else
        {
            bc->reg->CommandWord1 |=
                    // Количество слов выдаваемых данных
                    MIL_STD_COMWORD_WORDSCNT_CODE(currSlot->words_count) |
                    // Подадрес источника
                    MIL_STD_COMWORD_SUBADDR_MODE(currSlot->subaddr_source) |
                    // Адрес источника
                    MIL_STD_COMWORD_ADDR(currSlot->addr_source);
            bc->reg->CommandWord2 |=
                    // Количество слов принимаемых данных
                    MIL_STD_COMWORD_WORDSCNT_CODE(currSlot->words_count) |
                    // Подадрес приёмника
                    MIL_STD_COMWORD_SUBADDR_MODE(currSlot->subaddr_dest) |
                    // Направление передачи: ОУ-ОУ
                    MIL_STD_COMWORD_RT_BC |
                    // Адрес приёмника
                    MIL_STD_COMWORD_ADDR(currSlot->addr_dest);

#if BC_DEBUG
            debug_printf("rt->rt, slot = %x, %x, %x, %x, %x\n",
                         currSlot->addr_source,
                         currSlot->subaddr_source,
                         currSlot->addr_dest,
                         currSlot->subaddr_dest,
                         currSlot->words_count);
#endif
        }

        // Инициировать передачу команды в канал в режиме КШ
        bc->reg->CONTROL |= MIL_STD_CONTROL_BCSTART;
    }
    else
    {
#if BC_DEBUG
        debug_printf("empty\n");
#endif
    }

    // Перейти к следующему слоту в циклограмме.
    bc->curr_slot = (bc->curr_slot + 1) % bc->slots_count;

    ARM_TIMER1->TIM_STATUS = 1;

    arch_intr_allow(timer_irq);

    return 0;
}

static void my_timer_init(unsigned int khz, unsigned int msec_per_tick, mil_std_bc_t *bc)
{
    // Подача синхроимпульсов для таймера
    ARM_RSTCLK->PER_CLOCK |= ARM_PER_CLOCK_TIMER1;

    // Регистр TIM_CLOCK: управление тактовой частотой таймеров
    ARM_RSTCLK->TIM_CLOCK = ARM_TIM_CLOCK_BRG1(0) | ARM_TIM_CLOCK_EN1;

    ARM_TIMER1->TIM_CNTRL = 0x00000000;

    // Начальное значение счетчика
    ARM_TIMER1->TIM_CNT = 0x00000000;
    // Предделитель частоты
    ARM_TIMER1->TIM_PSG = 0x00000000;
    // Основание счета
    ARM_TIMER1->TIM_ARR = msec_per_tick*khz;

    // Разрешение генерировать прерывание при CNT = ARR
    ARM_TIMER1->TIM_IE = 0x00000002;

    // Счет вверх по TIM_CLK. Разрешение работы таймера.
    ARM_TIMER1->TIM_CNTRL = 0x00000001;

    // Attach fast handler to timer interrupt.
    mutex_attach_irq(&timerMutex, timer_irq, mil_std_1553_timer_handler, bc);
}

int mil_std_1553_bc_init(mil_std_bc_t *bc,
                         int port,
                         const cyclogram_slot_t *cyclogram,
                         int slot_time,
                         int slots_count,
                         int cpu_freq,
                         unsigned short *rx_buf,
                         unsigned short *tx_buf)
{
    if (mil_std_1553_setup(port, MIL_STD_MODE_BC, 0) < 0)
        return -1;

    bc->port = port;
    bc->slots_count = slots_count;
    bc->curr_slot = 0;
    bc->rt_bc_slot = 0xff;
    bc->reg = port == 0 ? ARM_MIL_STD_1553B1 : ARM_MIL_STD_1553B2;
    bc->cyclogram = cyclogram;
    bc->rx_buf = rx_buf;
    bc->tx_buf = tx_buf;

    mutex_init(&bc->lock);

    int locIrqNum = port == 0 ? MIL_STD_1553B1_IRQn : MIL_STD_1553B2_IRQn;

    // Настроить работу MIL-STD по прерыванию, указать функцию обработчик прерывания.
    mutex_attach_irq(&milstdMutex,
                     locIrqNum,
                     mil_std_1553_bc_handler,
                     bc);

    my_timer_init(cpu_freq, slot_time, bc);

    // Разрешить прерывания:
    // при приёме достоверного слова,
    // при возникновении ошибки в сообщении.
    bc->reg->INTEN =
            MIL_STD_INTEN_RFLAGNIE |
            MIL_STD_INTEN_ERRIE;

    return 0;
}

#endif

