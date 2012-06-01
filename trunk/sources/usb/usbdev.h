#ifndef __USBDEV_H__
#define __USBDEV_H__

#include <runtime/lib.h>
#include <kernel/uos.h>
#include <mem/mem-queue.h>
#include "usb_const.h"
#include "usb_struct.h"

//
// Настроечные параметры стека USB-устройства.
// Для экономия ОЗУ рекомендуется задавать минимально необходимые значения.
//

// Максимальное количество одновременно задействованных интерфейсов
// (наибольшее из количеств интерфейсов по каждой конфигурации)
#ifndef USBDEV_NB_INTERFACES
#define USBDEV_NB_INTERFACES        1
#endif

// Количество используемых конечных точек
#ifndef USBDEV_NB_ENDPOINTS
#define USBDEV_NB_ENDPOINTS         16
#endif

// Количество конфигураций устройства
#ifndef USBDEV_NB_CONF
#define USBDEV_NB_CONF              1
#endif

// Размер максимальной длины передачи для нулевой конечной точки
#ifndef USBDEV_EP0_MAX_SIZE
#define USBDEV_EP0_MAX_SIZE         8
#endif

// Глубина приёмных очередей по умолчанию
#ifndef USBDEV_DEFAULT_RXQ_DEPTH
#define USBDEV_DEFAULT_RXQ_DEPTH    2
#endif

//
// Направления конечных точек
// Endpoint directions
//
#define USBDEV_DIR_OUT              0
#define USBDEV_DIR_IN               1

struct _usbdev_t;
struct _usbdev_hal_t;
typedef struct _usbdev_t usbdev_t;
typedef struct _usbdev_hal_t usbdev_hal_t;

//
// Функции, вызываемые стеком USBDEV (должны быть реализованы аппаратным драйвером (HAL)
// Functions called by USBDEV stack (to be realized by HAL)
//

// Установка адреса устройства.
// Эта функция должна установить адрес устройства, указанный в параметре addr.
typedef void (*usbdev_set_addr_func_t) (unsigned addr);

// Установка атрибутов конечной точки.
// Номер конечной точки указан в параметре ep, направление - в параметре dir.
// Параметр dir может принимать значения USBDEV_DIR_OUT или USBDEV_DIR_IN.
// attr - требуемые атрибуты конечной точки (TODO: описать атрибуты).
// max_size - максимальный размер передачи.
// interval - интервал передачи (имеет значение только для изохронных конечных точек).
typedef void (*usbdev_set_ep_attr_func_t) (unsigned ep, int dir, unsigned attr, int max_size, int interval);

// Функция передачи.
// Данная функция должна настроить выдачу массива информации, указанного в параметре data,
// размера size через конечную точку с номером ep. PID пакета должен быть установлен в
// значение, указанное в параметре pid.
typedef void (*usbdev_tx_func_t) (unsigned ep, int pid, const void *data, int size);

// Функция определения имеющегося свободного места в передатчике.
// Данная функция должна возвращать, сколько ещё байт можно выдать в конечную точку с
// номером ep.
typedef int  (*usbdev_tx_avail_func_t) (unsigned ep);

// Функция блокировки конечной точки.
// Данная функция должна заблокировать конечную точку с номером ep и направлением dir
// (установить для указанной точки статус STALLED).
// Параметр dir может принимать значения USBDEV_DIR_OUT или USBDEV_DIR_IN.
typedef void (*usbdev_stall_func_t) (unsigned ep, int dir);

// Прототип функции-обработчика запросов, специфичных для класса.
// Обработчик регистрируется в стеке вызовом usbdev_set_class_handler.
typedef void (*usbdev_specific_t) (void *tag, usb_setup_pkt_t *setup_pkt, unsigned char **data, int *size);

// Структура с функциями, вызываемыми стеком USBDEV.
struct _usbdev_hal_t
{

    usbdev_set_addr_func_t      set_addr;
    usbdev_set_ep_attr_func_t   ep_attr;
    usbdev_tx_func_t            tx;
    usbdev_tx_avail_func_t      tx_avail;
    usbdev_stall_func_t         stall;
};

//
// Внутренняя управляющая структура одной конечной точки (сразу для двух направлений
// конечной точки - IN и OUT). Не рекомендуется использовать информацию из данной
// структуры в приложении или в аппаратном драйвере.
// Endpoint control structure
//
typedef struct _ep_ctrl_t {
    mutex_t         lock;           // Мьютекс для синхронизации доступа
    int             state;          // Состояние конечной точки
    int             attr[2];        // Атрибуты конечной точки (OUT и IN), взятые из 
                                    // дескриптора(-ов) конечной(-ых) точки(-ек)
    int             max_size[2];    // Максимальный размер передачи для конечных точек
                                    // OUT и IN
    int             interval[2];    // Интервал (для изохронных конечных точек)
    const uint8_t * in_ptr;         // Текущий указатель, из которого выдаётся
                                    // очередная порция сообщения
    int             in_rest_load;   // Количество байтов, которое осталось выдать в
                                    // текущей передаче
    int             in_rest_ack;    // Количество байтов, которое осталось подтвердить.
                                    // Данное число увеличивается в момент получения ACK от хоста
    int             pid;            // PID для следующей выдачи
    mem_queue_t     rxq;            // Приёмная очередь
    int             rxq_depth;      // Глубина приёмной очереди
    
    usbdev_specific_t   in_specific_handler;
    void *              in_specific_tag;
    usbdev_specific_t   out_specific_handler;
    void *              out_specific_tag;
} ep_ctrl_t;

typedef struct _iface_ctrl_t {
    usbdev_specific_t   specific_handler;
    void *              specific_tag;
} iface_ctrl_t;

//
// USB device driver structure
//
struct __attribute__ ((packed)) _usbdev_t
{
    usb_dev_desc_t *        dev_desc;
    usb_conf_desc_t *       conf_desc [USBDEV_NB_CONF];
    void **                 strings;
    unsigned                cur_conf;
    unsigned                usb_addr;
    
    mem_pool_t *            pool;
    usbdev_hal_t *          hal;
    
    ep_ctrl_t               ep_ctrl [USBDEV_NB_ENDPOINTS];
    iface_ctrl_t            iface_ctrl [USBDEV_NB_INTERFACES];
    int                     state;
    
    usbdev_specific_t       dev_specific_handler;
    void *                  dev_specific_tag;
    
    // Statistics
    unsigned                rx_discards;
    unsigned                rx_req;
    unsigned                rx_bad_req;
    unsigned                rx_bad_pid;
    unsigned                rx_bad_len;
    unsigned                ctrl_failed;
    unsigned                out_of_memory;    
};


//
// Функции обратного вызова. Вызываются аппаратным драйвером.
// Interface for HAL (callbacks). Do not call from user code!
//

// Привязка аппаратного драйвера к стеку. Функция должна вызываться
// в момент инициализации аппаратного драйвера. Функция передаёт в 
// параметре hal в стек структуру с функциями, которые затем будут 
// вызываться стеком.
void usbdevhal_bind (usbdev_t *u, usbdev_hal_t *hal);

// Эту функцию должен вызвать аппаратный драйвер по событию сброса шины USB.
void usbdevhal_reset (usbdev_t *u);

// Эту функцию должен вызвать аппаратный драйвер в случае определения состояния IDLE
// в шине USB.
void usbdevhal_suspend (usbdev_t *u);

// Эту функцию должен вызвать аппаратный драйвер по окончанию выдачи пакета хосту.
// ep - номер конечной точки, size - размер выданного пакета.
void usbdevhal_tx_done (usbdev_t *u, unsigned ep, int size);

// Эту функцию должен вызвать аппаратный драйвер по окончанию приёма пакета от хосту.
// ep - номер конечной точки, pid - PID принятого пакета, data - указатель на
// буфер с принятым пакетом, size - размер пакета.
void usbdevhal_rx_done (usbdev_t *u, unsigned ep, int pid, void *data, int size);

//
// USB device API
//
void usbdev_init (usbdev_t *u, mem_pool_t *pool, const usb_dev_desc_t *dd);
void usbdev_add_config_desc (usbdev_t *u, const void *cd);
void usbdev_set_string_table (usbdev_t *u, const void *st[]);
void usbdev_set_dev_specific_handler (usbdev_t *u, usbdev_specific_t handler, void *tag);
void usbdev_set_iface_specific_handler (usbdev_t *u, unsigned if_n, usbdev_specific_t handler, void *tag);
void usbdev_set_ep_specific_handler (usbdev_t *u, unsigned ep_n, int dir, usbdev_specific_t handler, void *tag);
void usbdev_set_rx_queue_depth (usbdev_t *u, unsigned ep, int depth);
void usbdev_send (usbdev_t *u, unsigned ep, const void *data, int size);
int  usbdev_recv (usbdev_t *u, unsigned ep, void *data, int size);

#endif
