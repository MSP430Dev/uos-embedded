/*
 * Описание регистров контроллера КСК.
 *
 * Автор: Дмитрий Подхватилин, НПП "Дозор" 2011, 2012
 */
 
#ifndef _KSK_REG_H_
#define _KSK_REG_H_

/* !!! ВНИМАНИЕ !!!
 *
 * Для использования данного заголовочного файла необходимо задать
 * базовый адрес КСК в адресном пространстве процессора - константу 
 * KSK_MEM_BASE перед включением данного файла в текст программы. 
 * Например,
 * #define KSK_MEM_BASE	0x10000000
 * #include <dozor/ksk-reg.h>
 */
 
#define KSK_MEM_SIZE    0x8000
 
#ifndef vu32
#define vu32 volatile uint32_t
#endif

#define KSK_CW1         (*((vu32 *) KSK_MEM_BASE))                      // Управляющее слово 1
#define KSK_CW2         (*((vu32 *) KSK_MEM_BASE + 511))                // Управляющее слово 2
#define KSK_SW1(n)      (*((vu32 *) KSK_MEM_BASE + (n) * 512))          // Слово состояния 1 (для каждого абонента)
#define KSK_SW2(n)      (*((vu32 *) KSK_MEM_BASE + (n) * 512 + 511))    // Слово состояния 2 (для каждого абонента)
#define KSK_DATA(n,a)   (*((vu32 *) KSK_MEM_BASE + (n) * 512 + (a)))    // Слово данных, n - номер области (абонента), a - адрес слова

/***************************
 * Управляющие слова 1 и 2 *
 ***************************/
#define KSK_ADDRESS(n)              ((n) & 0x1FF)         /* Стартовый адрес данных, от начала области передатчика */
#define KSK_WORDS_PER_CYCLE(n)      (((n) & 0x1FF) << 9)  /* Количество слов, выдаваемых в одном цикле системы */
#define KSK_WORDS_PER_ROUND(n)      (((n) & 0x1FF) << 18) /* Количество слов, выдаваемых в одном раунде TDMA */
#define KSK_KEY(n)                  (((n) & 0x1F) << 27)  /* Поле ключа для запуска */

#define KSK_START_KEY               KSK_KEY(0x15)

/*************************
 * Слова состояния 1 и 2 *
 *************************/
#define KSK_GET_RX1(reg)            ((reg) & 0xFF)
#define KSK_GET_RX2(reg)            (((reg) >> 8) & 0xFF)
#define KSK_GET_RX3(reg)            (((reg) >> 16) & 0xFF)
#define KSK_GET_PLACE(reg)          (((reg) >> 24) & 0xF)
#define KSK_GRUN                    (1 << 28)

#endif /*_KSK_REG_H_*/

