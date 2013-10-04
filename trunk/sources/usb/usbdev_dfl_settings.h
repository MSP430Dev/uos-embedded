#ifndef __USBDEV_DFL_SETTINGS_H__
#define __USBDEV_DFL_SETTINGS_H__

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

// Признак запитки от собственного источника
// (в противоположность запитке от шины)
#ifndef USBDEV_SELF_POWERED
#define USBDEV_SELF_POWERED         1
#endif

// Признак того, что устройство готово принять запрос на
// удалённое пробуждение
#ifndef USBDEV_REMOTE_WAKEUP
#define USBDEV_REMOTE_WAKEUP        0
#endif

#endif
