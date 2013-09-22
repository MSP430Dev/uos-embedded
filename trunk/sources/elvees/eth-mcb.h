/*
 * Ethernet controller driver for Elvees Multicore NVCom.
 * Copyright (c) 2010 Serge Vakulenko.
 *               2013 Dmitry Podkhvatilin, <vatilin@gmail.com>
 * Based on sources from Ildar F Kaibyshev skif@elvees.com.
 *
 * This file is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can redistribute this file and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software Foundation;
 * either version 2 of the License, or (at your discretion) any later version.
 * See the accompanying file "COPYING.txt" for more details.
 *
 * As a special exception to the GPL, permission is granted for additional
 * uses of the text contained in this file.  See the accompanying file
 * "COPY-UOS.txt" for details.
 */
#ifndef __NVCOM_ETH_H
#define __NVCOM_ETH_H

#include <net/netif.h>
#include <buf/buf-queue.h>

#ifndef ETH_STACKSZ
#   define ETH_STACKSZ      1000
#endif

#ifndef ETH_INQ_SIZE
#   define ETH_INQ_SIZE     16
#endif

#ifndef ETH_OUTQ_SIZE
#   define ETH_OUTQ_SIZE    8
#endif

#ifndef ETH_MTU
#   define ETH_MTU      1518    /* maximum ethernet frame length */
#endif

struct _mem_pool_t;
struct _stream_t *stream;

typedef struct _eth_t {
    netif_t netif;                  /* common network interface part */
    mutex_t tx_lock;                /* get tx interrupts here */
    struct _mem_pool_t *pool;       /* memory pool for allocating packets */

    buf_queue_t inq;                /* queue of received packets */
    struct _buf_t *inqdata[ETH_INQ_SIZE];

    buf_queue_t outq;               /* queue of packets to transmit */
    struct _buf_t *outqdata[ETH_OUTQ_SIZE];

    unsigned phy;                   /* address of external PHY */
    unsigned long intr;             /* interrupt counter */
    unsigned char *rxbuf;           /* aligned rxbuf[] */
    unsigned char *txbuf;           /* aligned txbuf[] */
    unsigned rxbuf_physaddr;        /* phys address of rxbuf[] */
    unsigned txbuf_physaddr;        /* phys address of txbuf[] */

    ARRAY (stack, ETH_STACKSZ);     /* stack for receive task */
} eth_mcb_t;

//
// Инициализация драйвера Ethernet
//
//  u       указатель на структуру, описывающую интерфейс
//  name    символическое имя интерфейса
//  prio    приоритет задачи-обработчика прерываний по выдаче
//          Приоритет задачи-обработчика прерываний по приёму
//          принимается равным (prio + 1)
//  pool    указатель на пул динамической памяти, используемый
//          драйвером Ethernet
//  arp     указатель на структуру ARP
//  macaddr указатель на массив 6 байтов, содержащий MAC-адрес
//          устройства
//
void eth_mcb_init (eth_mcb_t *u, const char *name, int prio,
    struct _mem_pool_t *pool, struct _arp_t *arp, const unsigned char *macaddr);

//
// Вывод отладочной информации по драйверу Ethernet
//
//  u       указатель на структуру, описывающую интерфейс
//  stream  указатель на поток, в который выдавать отладочную
//          информацию
//
void eth_mcb_debug (eth_mcb_t *u, struct _stream_t *stream);

//
// Выполнить определение параметров сети
//
//  u       указатель на структуру, описывающую интерфейс
//
void eth_mcb_start_negotiation (eth_mcb_t *u);

//
// Определить наличие несущей в линии
//
//  u       указатель на структуру, описывающую интерфейс
//
int eth_mcb_get_carrier (eth_mcb_t *u);

//
// Определить текущие настройки скорости
//
//  u       указатель на структуру, описывающую интерфейс
//  duplex  указатель на переменную, в которую будет помещен
//          признак дуплексного режима
//  возвращает скорость в бит/сек
long eth_mcb_get_speed (eth_mcb_t *u, int *duplex);

//
// Установить внутреннюю петлю в PHY для самотестирования устройства
//
//  u       указатель на структуру, описывающую интерфейс
//  on      1 - установить петлю, 0 - отключить петлю
//
void eth_mcb_set_phy_loop (eth_mcb_t *u, int on);

//
// Установить внутреннюю петлю в MAC для самотестирования устройства
//
//  u       указатель на структуру, описывающую интерфейс
//  on      1 - установить петлю, 0 - отключить петлю
//
void eth_mcb_set_mac_loop (eth_mcb_t *u, int on);

//
// Установить параметры режима прослушивания сети
//
//  u       указатель на структуру, описывающую интерфейс
//  station узел
//  group   группа
void eth_mcb_set_promisc (eth_mcb_t *u, int station, int group);

//
// Опросить устройство
//
//  u       указатель на структуру, описывающую интерфейс
//
void eth_mcb_poll (eth_mcb_t *u);

#endif /* __NVCOM_ETH_H */
