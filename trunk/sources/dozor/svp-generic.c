#include <runtime/lib.h>
#include <kernel/uos.h>
#include <kernel/internal.h>
#include <stream/stream.h>
#include "svp.h"

#ifdef SVP_VER_C1
#include "svp-reg-c1.h"
#else
#include "svp-reg.h"
#endif

#if !defined(SVP_BUS_ONLY) && !defined(SVP_SPI_ONLY)

uint16_t svp_read16 (SVP_T *psvp, uint16_t addr)
{
    svp_generic_t *psvp_gen = psvp;
    if (psvp_gen->over_spi) return svps_read16 ((svps_t *)psvp, addr);
    else return svpb_read16 ((svpb_t *)psvp, addr);
}

uint32_t svp_read32 (SVP_T *psvp, uint16_t addr)
{
    svp_generic_t *psvp_gen = psvp;
    if (psvp_gen->over_spi) return svps_read32 ((svps_t *)psvp, addr);
    else return svpb_read32 ((svpb_t *)psvp, addr);
}

void svp_read_array (SVP_T *psvp, uint16_t addr, void *buf, int size)
{
    svp_generic_t *psvp_gen = psvp;
    if (psvp_gen->over_spi) svps_read_array ((svps_t *)psvp, addr, buf, size);
    else return svpb_read_array ((svpb_t *)psvp, addr, buf, size);
}

void svp_write16 (SVP_T *psvp, uint16_t addr, uint16_t data)
{
    svp_generic_t *psvp_gen = psvp;
    if (psvp_gen->over_spi) svps_write16 ((svps_t *)psvp, addr, data);
    else svpb_write16 ((svpb_t *)psvp, addr, data);
}

void svp_write32 (SVP_T *psvp, uint16_t addr, uint32_t data)
{
    svp_generic_t *psvp_gen = psvp;
    if (psvp_gen->over_spi) svps_write32 ((svps_t *)psvp, addr, data);
    else svpb_write32 ((svpb_t *)psvp, addr, data);
}

void svp_write_array (SVP_T *psvp, uint16_t addr, const void *buf, int size)
{
    svp_generic_t *psvp_gen = psvp;
    if (psvp_gen->over_spi) svps_write_array ((svps_t *)psvp, addr, buf, size);
    else svpb_write_array ((svpb_t *)psvp, addr, buf, size);
}
#endif

static bool_t svp_interrupt_handler (SVP_T *arg)
{
    svp_generic_t * psvp_gen = (svp_generic_t *)arg;
    if (psvp_gen->int_handler)
        psvp_gen->int_handler (psvp_gen);
    arch_intr_allow (psvp_gen->irq);
    return 0;
}

void svp_set_int_handler (SVP_T *psvp, int irq, svp_int_handler_t ih, int positive)
{
    svp_generic_t *psvp_gen = (svp_generic_t *)psvp;
    
    unsigned gcr = svp_read16(psvp, SVP_GCR);
    if (positive) gcr |= SVP_IRQP;
	else gcr &= ~SVP_IRQP;
	gcr |= SVP_IEN;
    svp_write16 (psvp, SVP_GCR, gcr);
    
    psvp_gen->irq = irq;
    psvp_gen->int_handler = ih;
    mutex_attach_irq (&psvp_gen->lock, irq, (handler_t)svp_interrupt_handler, psvp);
}

void svp_unset_int_handler (SVP_T *psvp)
{
    svp_generic_t *psvp_gen = (svp_generic_t *)psvp;
    mutex_lock (&psvp_gen->lock);
    mutex_unlock_irq (&psvp_gen->lock);
    psvp_gen->irq = 0;
    psvp_gen->int_handler = 0;
}

void svp_reset (SVP_T *psvp)
{
    svp_generic_t *psvp_gen = (svp_generic_t *)psvp;
    
    mutex_lock (&psvp_gen->lock);
    // Оставляем в GCR только флаги IRQP и IEN, если они были
    unsigned gcr = svp_read16(psvp, SVP_GCR) & (SVP_IRQP | SVP_IEN);
    svp_write16 (psvp, SVP_GCR, SVP_GRST);
    udelay (1);
	svp_write16 (psvp, SVP_GCR, gcr);
	// Сброс флагов прерываний
	svp_write16 (psvp, SVP_GSR, 0xFFFF);
	svp_write16 (psvp, SVP_RSR(0), 0xFFFF);
	svp_write16 (psvp, SVP_RSR(1), 0xFFFF);
    mutex_unlock (&psvp_gen->lock);
}

#ifdef SVP_VER_C1
void svp_init_node (SVP_T *psvp, svp_init_params_t *p)
{
    svp_generic_t *psvp_gen = (svp_generic_t *)psvp;
    
    mutex_lock (&psvp_gen->lock);
    
	svp_write16 (psvp, SVP_NID, p->node_id);
	svp_write32 (psvp, SVP_SID, p->medl_id);
	svp_write16 (psvp, SVP_TPLR, p->preamble_len);
	svp_write16 (psvp, SVP_BDR, p->speed);
	//svp_write_array (psvp, SVP_MEDL_BASE, p->medl, p->medl_size + 16);
	svp_write_array (psvp, SVP_MEDL_BASE, p->medl, p->medl_size);
	svp_write16 (psvp, SVP_SSR, SVP_MEDL_BASE);
	svp_write32 (psvp, SVP_MTR, p->cycle_duration);
	svp_write32 (psvp, SVP_NMTR, p->cycle_duration);
	svp_write16 (psvp, SVP_CMR, SVP_MR_STRT | SVP_MR_RXEN0 | SVP_MR_RXEN1 | SVP_MR_TXEN0 | SVP_MR_TXEN1 | 0x11);
	svp_write16 (psvp, SVP_NMR, SVP_MR_RXEN0 | SVP_MR_RXEN1 | SVP_MR_TXEN0 | SVP_MR_TXEN1 | 0x11);
	svp_write16 (psvp, SVP_RADS(0), 0);
	svp_write16 (psvp, SVP_RADS(1), SVP_DATA_SIZE / 2);
	
	unsigned gcr = svp_read16(psvp, SVP_GCR);
	gcr |= SVP_LC(4) | SVP_HW_START;
	if (p->master) gcr |= SVP_MST;
	if (p->cluster_size == SVP_CLUSTER_64) gcr |= SVP_N64;
	else if (p->cluster_size == SVP_CLUSTER_32) gcr |= SVP_N32;
	svp_write16(psvp, SVP_GCR, gcr);
	
	mutex_unlock (&psvp_gen->lock);
}
#else
void svp_init_node (SVP_T *psvp, svp_init_params_t *p)
{
    svp_generic_t *psvp_gen = (svp_generic_t *)psvp;

    mutex_lock (&psvp_gen->lock);

    /* Номер узла в кластере. */
    svp_write16 (psvp, SVP_NID, p->node_id);

    /* Уникальный идентификатор расписания. */
    svp_write32 (psvp, SVP_SID, p->medl_id);

    /* Запись таблицы расписания. */
    svp_write_array (psvp, SVP_MODE_BASE, p->mode_table, sizeof(p->mode_table));
    svp_write_array (psvp, SVP_DELAY_BASE, p->delay_table, sizeof(p->delay_table));
    svp_write_array (psvp, SVP_MEDL_BASE, p->medl, sizeof(p->medl));

    /* Время до выдачи собственных стартовых пакетов */
    svp_write32 (psvp, SVP_SPDR, p->spdr);

    /* Подготовка GCR, но без пуска устройства */
    unsigned gcr = svp_read16(psvp, SVP_GCR);
    gcr |= SVP_LC(4);
    if (p->cluster_size == SVP_CLUSTER_64) gcr |= SVP_N64;
    else if (p->cluster_size == SVP_CLUSTER_32) gcr |= SVP_N32;

    /* Если установлен старший бит в номере узла, то узел должен работать как мастер */
    if (p->master) {
        svp_write16 (psvp, SVP_CMR, (p->pref_mode << 4) | p->start_mode);
        gcr |= SVP_MST;
    } else {
        svp_write16 (psvp, SVP_CMR, (p->start_mode << 4) | p->start_mode);
    }
    svp_write16 (psvp, SVP_NMR, (p->start_mode << 4) | p->start_mode);

    svp_write16(psvp, SVP_GCR, gcr);

    mutex_unlock (&psvp_gen->lock);
}
#endif

void svp_start (SVP_T *psvp)
{
	svp_write16(psvp, SVP_GCR, svp_read16(psvp, SVP_GCR) | SVP_GRUN);
}

