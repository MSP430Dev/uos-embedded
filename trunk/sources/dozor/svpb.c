#include <runtime/lib.h>
#include <kernel/uos.h>
#include <kernel/internal.h>
#include "svp.h"

void svpb_init (svpb_t *svp, uint32_t base_addr)
{
    svp->svp_gen.over_spi = 0;
    svp->svp_gen.irq = -1;
    svp->svp_gen.int_handler = 0;
    svp->base_addr = base_addr;
}

uint32_t svpb_read32 (svpb_t *svp, uint16_t addr)
{
    unsigned received;
    received = svpb_read16 (svp, addr);
    received |= svpb_read16 (svp, addr + 2) << 16;
    return received;
}

void svpb_read_array (svpb_t *svp, uint16_t addr, void *buf, int size)
{
    uint16_t *p = (uint16_t *) buf;
    int i = 0;

    while (size > 0) {
        p [i++] = svpb_read16 (svp, addr);
        addr += 2;
        size -= 2;
    }
}

void svpb_write32 (svpb_t *svp, uint16_t addr, uint32_t data)
{
    svpb_write16 (svp, addr, data);
    svpb_write16 (svp, addr + 2, data >> 16);
}

void svpb_write_array (svpb_t *svp, uint16_t addr, const void *buf, int size)
{
    uint16_t *pbuf = (uint16_t *) buf;
    int i = 0;

    while (size > 0) {
        svpb_write16 (svp, addr, pbuf [i++]);
        addr += 2;
        size -= 2;
    }
}

