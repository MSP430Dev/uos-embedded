#ifndef _SVP_H_
#define _SVP_H_

#include <spi/spi-master-interface.h>

#if defined(SVP_VER_C1)
#include <dozor/svp-reg-c1.h>
#else
#include <dozor/svp-reg.h>
#endif

#define SVP_MAX_NODES   64
#define SVP_MAX_MODES   16

typedef struct _svp_generic_t svp_generic_t;
typedef void (*svp_int_handler_t) (svp_generic_t *);

struct _svp_generic_t
{
    mutex_t lock;
    int over_spi;
    int irq;
    svp_int_handler_t int_handler;
};

typedef struct _svpb_t
{
    svp_generic_t   svp_gen;
    uint32_t        base_addr;
} svpb_t;

typedef struct _svps_t
{
    svp_generic_t   svp_gen;
    spimif_t       *spi;            // Указатель на драйвер SPI
    spi_message_t   msg;            // Сообщение SPI
} svps_t;

#if defined(SVP_BUS_ONLY)
typedef svpb_t SVP_T;
#elif defined(SVP_SPI_ONLY)
typedef svps_t SVP_T;
#else
typedef void SVP_T;
#endif


enum
{
    SVP_10MBIT,
    SVP_5MBIT,
    SVP_3_33_MBIT,
    SVP_2_5_MBIT,
    SVP_2_MBIT,
    SVP_1_67_MBIT,
    SVP_1_43_MBIT,
    SVP_1_25_MBIT,
    SVP_1_11_MBIT,
    SVP_1_MBIT
};

enum
{
    SVP_CLUSTER_16,
    SVP_CLUSTER_32,
    SVP_CLUSTER_64
};

typedef struct _svp_init_params_t
{
    struct {
    unsigned    node_id         : 6;
    unsigned    preamble_len    : 8;
    unsigned    speed           : 4;
    unsigned    cluster_size    : 2;
    int         master          : 1;
    } __attribute__((packed));
    
    unsigned    cycle_duration;
    unsigned    medl_id;
    medl_t      *medl;
    unsigned    medl_size;
#if !defined(SVP_VER_C1)
    unsigned        spdr;
    cluster_mode_t  mode_table[SVP_MAX_MODES];
    uint8_t         delay_table[SVP_MAX_NODES/2];
    uint8_t         start_mode;
    uint8_t         pref_mode;
#endif
} svp_init_params_t;



void        svpb_init (svpb_t *svp, uint32_t base_addr);

static inline uint16_t  svpb_read16 (svpb_t *svp, uint16_t addr)
{ return *((volatile uint16_t *) (svp->base_addr + (addr << 1))); }

uint32_t    svpb_read32 (svpb_t *svp, uint16_t addr);
void        svpb_read_array (svpb_t *svp, uint16_t addr, void *buf, int size);

static inline void      svpb_write16 (svpb_t *svp, uint16_t addr, uint16_t data)
{ *((volatile uint16_t *) (svp->base_addr + (addr << 1))) = data; }

void        svpb_write32 (svpb_t *svp, uint16_t addr, uint32_t data);
void        svpb_write_array (svpb_t *svp, uint16_t addr, const void *buf, int size);

void        svps_init (svps_t *svp, spimif_t *spim, unsigned freq_hz);
uint16_t    svps_read16 (svps_t *svp, uint16_t addr);
uint32_t    svps_read32 (svps_t *svp, uint16_t addr);
void        svps_read_array (svps_t *svp, uint16_t addr, void *buf, int size);
void        svps_write16 (svps_t *svp, uint16_t addr, uint16_t data);
void        svps_write32 (svps_t *svp, uint16_t addr, uint32_t data);
void        svps_write_array (svps_t *svp, uint16_t addr, const void *buf, int size);


#if defined(SVP_BUS_ONLY)
#define svp_read16 svpb_read16
#define svp_read32 svpb_read32
#define svp_read_array svpb_read_array
#define svp_write16 svpb_write16
#define svp_write32 svpb_write32
#define svp_write_array svpb_write_array
#elif defined(SVP_SPI_ONLY)
#define svp_read16 svps_read16
#define svp_read32 svps_read32
#define svp_read_array svps_read_array
#define svp_write16 svps_write16
#define svp_write32 svps_write32
#define svp_write_array svps_write_array
#else
uint16_t    svp_read16 (SVP_T *psvp, uint16_t addr);
uint32_t    svp_read32 (SVP_T *psvp, uint16_t addr);
void        svp_read_array (SVP_T *psvp, uint16_t addr, void *buf, int size);
void        svp_write16 (SVP_T *psvp, uint16_t addr, uint16_t data);
void        svp_write32 (SVP_T *psvp, uint16_t addr, uint32_t data);
void        svp_write_array (SVP_T *psvp, uint16_t addr, const void *buf, int size);
#endif

void        svp_set_int_handler (SVP_T *psvp, int irq, svp_int_handler_t ih, int positive);
void        svp_unset_int_handler (SVP_T *psvp);
void        svp_reset (SVP_T *psvp);
void        svp_init_node (SVP_T *svp, svp_init_params_t *params);
void        svp_start (SVP_T *svp);

#endif /* _SVP_H_ */

