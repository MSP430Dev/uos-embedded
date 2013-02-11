#ifdef ARM_1986BE1

#include <runtime/lib.h>
#include <kernel/uos.h>
#include <kernel/internal.h>
#include <runtime/cortex-m1/io-1986ve1.h>
#include <milandr/mil-std-1553_setup.h>
#include <milandr/mil-std-1553_rt.h>
#include <milandr/mil-std-1553_bc.h>

#define RT_DEBUG 0

static bool_t mil_std_1553_rt_handler(void *arg)
{
    mil_std_rt_t *rt = (mil_std_rt_t *)arg;
    int locIrqNum = rt->port == 1 ? MIL_STD_1553B2_IRQn : MIL_STD_1553B1_IRQn;

    const unsigned short status = rt->reg->STATUS;
    const unsigned short comWrd1 = rt->reg->CommandWord1;
    const unsigned short msg = rt->reg->MSG;

    // Подадрес из командного слова 1
    const unsigned char subaddr = (comWrd1 & MIL_STD_COMWORD_SUBADDR_MODE_MASK) >> MIL_STD_COMWORD_SUBADDR_MODE_SHIFT;

    // Код команды
    const unsigned int cmdCode = (comWrd1 & MIL_STD_COMWORD_WORDSCNT_CODE_MASK) >> MIL_STD_COMWORD_WORDSCNT_CODE_SHIFT;

    // Количество слов данных в сообщении
    const unsigned char wordsCount = cmdCode == 0 ? 32 : cmdCode;

#if RT_DEBUG
    debug_printf("\nMSG=%x, ComWrd1=%x, subaddr=%x, wc=%x\n",
                 msg,
                 comWrd1,
                 subaddr,
                 wordsCount);
#endif

    unsigned short answerWord = MIL_STD_STATUS_ADDR_OU(rt->addr_self);
    switch (msg)
    {
    // приём данных от КШ (КШ-ОУ), формат сообщения 7
    case MSG_DATARECV__BC_RT__GROUP:
        answerWord |= MIL_STD_STATUS_GROUP_COMMAND_RECEIVED;
    // приём данных от КШ (КШ-ОУ), формат сообщения 1
    case MSG_DATARECV__BC_RT__SINGLE:
        // Получено достоверное слово из канала
        if ((status & MIL_STD_STATUS_RFLAGN) != 0)
        {
            // Установить ответное слово
            rt->reg->StatusWord1 = answerWord;

            int i;
            const int index = MIL_STD_SUBADDR_WORD_INDEX(subaddr);

#if RT_DEBUG
            debug_printf("STATUS1(F1,7)=%x, dataReceived =", status);
#endif

            mutex_lock(&rt->lock);
            for (i=0; i<wordsCount; ++i)
                rt->rx_buf[index + i] = rt->reg->DATA[index + i];
            mutex_unlock(&rt->lock);

#if RT_DEBUG
            for (i=0; i<wordsCount; ++i)
                debug_printf(" %x", rt->rx_buf[index + i]);
            debug_printf("\n");
#endif
        }
        break;
    // приём данных от ОУ (ОУ-ОУ), формат сообщения 8
    case MSG_DATARECV__RT_RT__GROUP:
        answerWord |= MIL_STD_STATUS_GROUP_COMMAND_RECEIVED;
    // приём данных от ОУ (ОУ-ОУ), формат сообщения 3
    case MSG_DATARECV__RT_RT__SINGLE:
        // Получено достоверное слово из канала
        if ((status & MIL_STD_STATUS_RFLAGN) != 0)
        {
            // Установить ответное слово
            rt->reg->StatusWord1 = answerWord;

            int i;
            int index = MIL_STD_SUBADDR_WORD_INDEX(subaddr);

#if RT_DEBUG
            debug_printf("STATUS(F3,8 in)=%x, dataReceived =", status);
#endif

            mutex_lock(&rt->lock);
            for (i=0; i<wordsCount; ++i)
                rt->rx_buf[index + i] = rt->reg->DATA[index + i];
            mutex_unlock(&rt->lock);

#if RT_DEBUG
            for (i=0; i<wordsCount; ++i)
                debug_printf(" %x", rt->rx_buf[index + i]);
            debug_printf("\n");
#endif
        }
        break;
    // передача данных в КШ (ОУ-КШ), формат сообщения 2
    case MSG_DATASEND__RT_BC__SINGLE:
        // Получено достоверное слово из канала
        if ((status & MIL_STD_STATUS_RFLAGN) != 0)
        {
            // Установить ответное слово
            rt->reg->StatusWord1 = answerWord;

            int i;
            int index = MIL_STD_SUBADDR_WORD_INDEX(subaddr);

#if RT_DEBUG
            debug_printf("STATUS(F2)=%x, dataToSend =", status);
            for (i=0; i<wordsCount; ++i)
                debug_printf(" %x", rt->tx_buf[index + i]);
            debug_printf("\n");
#endif

            mutex_lock(&rt->lock);
            for (i=0; i<wordsCount; ++i)
                rt->reg->DATA[index + i] = rt->tx_buf[index + i];
            mutex_unlock(&rt->lock);
        }
        break;
    // передача данных в ОУ (ОУ-ОУ), формат сообщения 8
    case MSG_DATASEND__RT_RT__GROUP:
        answerWord |= MIL_STD_STATUS_GROUP_COMMAND_RECEIVED;
    // передача данных в ОУ (ОУ-ОУ), формат сообщения 3
    case MSG_DATASEND__RT_RT__SINGLE:
        // Получено достоверное слово из канала
        if ((status & MIL_STD_STATUS_RFLAGN) != 0)
        {
            // Установить ответное слово
            rt->reg->StatusWord1 = answerWord;

#if RT_DEBUG
            debug_printf("STATUS(F3,8 out)=%x\n", status);
#endif

            // Подадрес из командного слова 2
            const unsigned char subaddr2 = (rt->reg->CommandWord2 & MIL_STD_COMWORD_SUBADDR_MODE_MASK) >> MIL_STD_COMWORD_SUBADDR_MODE_SHIFT;

            // Количество слов данных в сообщении
            int wcField2 = (rt->reg->CommandWord2 & MIL_STD_COMWORD_WORDSCNT_CODE_MASK) >> MIL_STD_COMWORD_WORDSCNT_CODE_SHIFT;
            const unsigned char wordsCount2 = wcField2 == 0 ? 32 : wcField2;

            int i;
            int index = MIL_STD_SUBADDR_WORD_INDEX(subaddr2);

            mutex_lock(&rt->lock);
            for (i=0; i<wordsCount2; ++i)
                rt->reg->DATA[index + i] = rt->tx_buf[index + i];
            mutex_unlock(&rt->lock);
        }
        break;
    // команда управления 0-15 от КШ без слов данных, формат сообщения 9
    case MSG_CMD_WITH_NODATA__0_0xf__GROUP:
        answerWord |= MIL_STD_STATUS_GROUP_COMMAND_RECEIVED;
    // команда управления 0-15 от КШ без слов данных, формат сообщения 4
    case MSG_CMD_WITH_NODATA__0_0xf__SINGLE:
        // Получено достоверное слово из канала
        if ((status & MIL_STD_STATUS_RFLAGN) != 0)
        {
            // неподдерживаемые команды (резерв)
            if (cmdCode >= 9 && cmdCode <= 0xf)
            {
                answerWord |= MIL_STD_STATUS_MSG_ERR;

                // Установить ответное слово
                rt->reg->StatusWord1 = answerWord;
            }
            else
            {
                // Установить ответное слово
                rt->reg->StatusWord1 = answerWord;

#if RT_DEBUG
                debug_printf("STATUS(F4,9)=%x, com=%x\n", status, cmdCode);
#endif

                // Команда "блокировать передатчик"
                if (cmdCode == CMD_LockSender)
                {
                    if ((status & MIL_STD_STATUS_RCVA) != 0)
                    {
                        rt->reg->CONTROL &= ~MIL_STD_CONTROL_TRA;
                    }
                    else if ((status & MIL_STD_STATUS_RCVB) != 0)
                        rt->reg->CONTROL &= ~MIL_STD_CONTROL_TRB;
                }
                // Команда "разблокировать передатчик"
                else if (cmdCode == CMD_UnlockSender)
                {
                    if ((status & MIL_STD_STATUS_RCVA) != 0)
                    {
                        rt->reg->CONTROL |= MIL_STD_CONTROL_TRA;
                    }
                    else if ((status & MIL_STD_STATUS_RCVB) != 0)
                        rt->reg->CONTROL |= MIL_STD_CONTROL_TRB;
                }
                // Команда "установить ОУ в исходное состояние"
                else if (cmdCode == CMD_SetRtInitialState)
                {

                }
            }
        }
        break;
    // команда управления 16-31 от КШ, ожидается слово данных (не групповая), формат сообщения 5
    case MSG_CMD_WITH_NODATA__0x10_0x1f__DATAWORD_EXPECTED__SINGLE:
        // Получено достоверное слово из канала
        if ((status & MIL_STD_STATUS_RFLAGN) != 0)
        {
            // неподдерживаемые команды (резерв)
            if (cmdCode >= 0x16)
            {
                answerWord |= MIL_STD_STATUS_MSG_ERR;

                // Установить ответное слово
                rt->reg->StatusWord1 = answerWord;
            }
            else
            {
                // Установить ответное слово
                rt->reg->StatusWord1 = answerWord;

#if RT_DEBUG
                debug_printf("STATUS(F5)=%x\n", status);
#endif

                // (!) to be implemented
            }
        }
        break;
    // команда управления 16-31 от КШ со словом данных, формат сообщения 10
    case MSG_CMD_WITH_DATA__0x10_0x1f__GROUP:
        answerWord |= MIL_STD_STATUS_GROUP_COMMAND_RECEIVED;
    // команда управления 16-31 от КШ со словом данных, формат сообщения 6
    case MSG_CMD_WITH_DATA__0x10_0x1f__SINGLE:
        // Получено достоверное слово из канала
        if ((status & MIL_STD_STATUS_RFLAGN) != 0)
        {
            // неподдерживаемые команды (резерв)
            if (cmdCode >= 0x16)
            {
                answerWord |= MIL_STD_STATUS_MSG_ERR;

                // Установить ответное слово
                rt->reg->StatusWord1 = answerWord;
            }
            else
            {
                // Установить ответное слово
                rt->reg->StatusWord1 = answerWord;

#if RT_DEBUG
                debug_printf("STATUS(F6,10)=%x\n", status);
#endif

                // Команда "синхронизация (со словом данных)"
                if (cmdCode == CMD_SynchronizeWithDataWord)
                {
                    // Заменить собственный адрес ОУ значением, которое получено в команде от КШ.
                    const unsigned short newCtrl = (rt->reg->CONTROL & ~MIL_STD_CONTROL_ADDR_MASK) | ((rt->reg->ModeData & 0x1F)<<MIL_STD_CONTROL_ADDR_SHIFT);
                    rt->reg->CONTROL = newCtrl;
                }
            }
        }
        break;
    }

    arch_intr_allow(locIrqNum);

    return 0;
}

void mil_std_1553_rt_init(mil_std_rt_t *rt, int port, int addr_self, unsigned short *rx_buf, unsigned short *tx_buf)
{
    MIL_STD_1553B_t *const mil_std_channel = mil_std_1553_port_setup(port);

    unsigned int locControl = 0;
    locControl |= MIL_STD_CONTROL_DIV(KHZ/1000);
    locControl |= MIL_STD_CONTROL_MODE(MIL_STD_MODE_RT);
    locControl |= MIL_STD_CONTROL_ADDR(addr_self) | MIL_STD_CONTROL_TRA | MIL_STD_CONTROL_TRB;
    mil_std_channel->StatusWord1 = MIL_STD_STATUS_ADDR_OU(addr_self);

    mil_std_channel->CONTROL = MIL_STD_CONTROL_MR;
    mil_std_channel->CONTROL = locControl;

    rt->addr_self = addr_self;
    rt->port = port;
    rt->reg = mil_std_channel;
    rt->rx_buf = rx_buf;
    rt->tx_buf = tx_buf;

    int locIrqNum = port == 1 ? MIL_STD_1553B2_IRQn : MIL_STD_1553B1_IRQn;

    // Настроить работу MIL-STD по прерыванию, указать функцию обработчик прерывания.
    mutex_attach_irq(&rt->lock,
                     locIrqNum,
                     mil_std_1553_rt_handler,
                     rt);

    // Разрешить прерывания:
    // при приёме достоверного слова,
    // при успешном завершении транзакции в канале,
    // при возникновении ошибки в сообщении.
    rt->reg->INTEN =
            MIL_STD_INTEN_RFLAGNIE |
            MIL_STD_INTEN_VALMESSIE |
            MIL_STD_INTEN_ERRIE;

    return 0;
}

#endif

