//
// Тест проверяет работу DAC с DMA Timer1.
// DMA используется в режиме "пинг-понг" для циклической генерации синуса 
// на выходе АЦП.
//

#include <runtime/lib.h>
#include <kernel/uos.h>
#include <timer/timer.h>

#define NB_OF_DMA_CHAN  32
#define DMA_CHAN        10

timer_t timer;
ARRAY(stack, 1000);

/*
// Пила
short dac_data[] = {
    0x000, 0x010, 0x020, 0x030, 0x040, 0x050, 0x060, 0x070,
    0x080, 0x090, 0x0A0, 0x0B0, 0x0C0, 0x0D0, 0x0E0, 0x0F0,
    0x100, 0x110, 0x120, 0x130, 0x140, 0x150, 0x160, 0x170,
    0x180, 0x190, 0x1A0, 0x1B0, 0x1C0, 0x1D0, 0x1E0, 0x1F0,
    0x200, 0x210, 0x220, 0x230, 0x240, 0x250, 0x260, 0x270,
    0x280, 0x290, 0x2A0, 0x2B0, 0x2C0, 0x2D0, 0x2E0, 0x2F0,
    0x300, 0x310, 0x320, 0x330, 0x340, 0x350, 0x360, 0x370,
    0x380, 0x390, 0x3A0, 0x3B0, 0x3C0, 0x3D0, 0x3E0, 0x3F0,
    0x000, 0x010, 0x020, 0x030, 0x040, 0x050, 0x060, 0x070,
    0x080, 0x090, 0x0A0, 0x0B0, 0x0C0, 0x0D0, 0x0E0, 0x0F0,
    0x100, 0x110, 0x120, 0x130, 0x140, 0x150, 0x160, 0x170,
    0x180, 0x190, 0x1A0, 0x1B0, 0x1C0, 0x1D0, 0x1E0, 0x1F0,
    0x200, 0x210, 0x220, 0x230, 0x240, 0x250, 0x260, 0x270,
    0x280, 0x290, 0x2A0, 0x2B0, 0x2C0, 0x2D0, 0x2E0, 0x2F0,
    0x300, 0x310, 0x320, 0x330, 0x340, 0x350, 0x360, 0x370,
    0x380, 0x390, 0x3A0, 0x3B0, 0x3C0, 0x3D0, 0x3E0, 0x3F0,
    0x000, 0x010, 0x020, 0x030, 0x040, 0x050, 0x060, 0x070,
    0x080, 0x090, 0x0A0, 0x0B0, 0x0C0, 0x0D0, 0x0E0, 0x0F0,
    0x100, 0x110, 0x120, 0x130, 0x140, 0x150, 0x160, 0x170,
    0x180, 0x190, 0x1A0, 0x1B0, 0x1C0, 0x1D0, 0x1E0, 0x1F0,
    0x200, 0x210, 0x220, 0x230, 0x240, 0x250, 0x260, 0x270,
    0x280, 0x290, 0x2A0, 0x2B0, 0x2C0, 0x2D0, 0x2E0, 0x2F0,
    0x300, 0x310, 0x320, 0x330, 0x340, 0x350, 0x360, 0x370,
    0x380, 0x390, 0x3A0, 0x3B0, 0x3C0, 0x3D0, 0x3E0, 0x3F0,
    0x000, 0x010, 0x020, 0x030, 0x040, 0x050, 0x060, 0x070,
    0x080, 0x090, 0x0A0, 0x0B0, 0x0C0, 0x0D0, 0x0E0, 0x0F0,
    0x100, 0x110, 0x120, 0x130, 0x140, 0x150, 0x160, 0x170,
    0x180, 0x190, 0x1A0, 0x1B0, 0x1C0, 0x1D0, 0x1E0, 0x1F0,
    0x200, 0x210, 0x220, 0x230, 0x240, 0x250, 0x260, 0x270,
    0x280, 0x290, 0x2A0, 0x2B0, 0x2C0, 0x2D0, 0x2E0, 0x2F0,
    0x300, 0x310, 0x320, 0x330, 0x340, 0x350, 0x360, 0x370,
    0x380, 0x390, 0x3A0, 0x3B0, 0x3C0, 0x3D0, 0x3E0, 0x3F0,
    0x000, 0x010, 0x020, 0x030, 0x040, 0x050, 0x060, 0x070,
    0x080, 0x090, 0x0A0, 0x0B0, 0x0C0, 0x0D0, 0x0E0, 0x0F0,
    0x100, 0x110, 0x120, 0x130, 0x140, 0x150, 0x160, 0x170,
    0x180, 0x190, 0x1A0, 0x1B0, 0x1C0, 0x1D0, 0x1E0, 0x1F0,
    0x200, 0x210, 0x220, 0x230, 0x240, 0x250, 0x260, 0x270,
    0x280, 0x290, 0x2A0, 0x2B0, 0x2C0, 0x2D0, 0x2E0, 0x2F0,
    0x300, 0x310, 0x320, 0x330, 0x340, 0x350, 0x360, 0x370,
    0x380, 0x390, 0x3A0, 0x3B0, 0x3C0, 0x3D0, 0x3E0, 0x3F0,
    0x000, 0x010, 0x020, 0x030, 0x040, 0x050, 0x060, 0x070,
    0x080, 0x090, 0x0A0, 0x0B0, 0x0C0, 0x0D0, 0x0E0, 0x0F0,
    0x100, 0x110, 0x120, 0x130, 0x140, 0x150, 0x160, 0x170,
    0x180, 0x190, 0x1A0, 0x1B0, 0x1C0, 0x1D0, 0x1E0, 0x1F0,
    0x200, 0x210, 0x220, 0x230, 0x240, 0x250, 0x260, 0x270,
    0x280, 0x290, 0x2A0, 0x2B0, 0x2C0, 0x2D0, 0x2E0, 0x2F0,
    0x300, 0x310, 0x320, 0x330, 0x340, 0x350, 0x360, 0x370,
    0x380, 0x390, 0x3A0, 0x3B0, 0x3C0, 0x3D0, 0x3E0, 0x3F0,
};
*/

// Синус
short dac_data[] = {
2048, 2249, 2447, 2642, 2831, 3012, 3185, 3346, 3495, 3630, 3749, 3853, 3939, 4006, 4055, 4085, 4095, 4085, 4056, 4007, 3940, 3854, 3751, 3632, 3497, 3349, 3187, 3015, 2834, 2645, 2450, 2252, 2051, 1851, 1652, 1457, 1268, 1086,  914,  752,  603,  468,  348,  245,  159,   90,   41,   11,    1,   10,   39,   88,  155,  240,  343,  462,  597,  745,  906, 1078, 1259, 1448, 1642, 1841,
2048, 2249, 2447, 2642, 2831, 3012, 3185, 3346, 3495, 3630, 3749, 3853, 3939, 4006, 4055, 4085, 4095, 4085, 4056, 4007, 3940, 3854, 3751, 3632, 3497, 3349, 3187, 3015, 2834, 2645, 2450, 2252, 2051, 1851, 1652, 1457, 1268, 1086,  914,  752,  603,  468,  348,  245,  159,   90,   41,   11,    1,   10,   39,   88,  155,  240,  343,  462,  597,  745,  906, 1078, 1259, 1448, 1642, 1841,
2048, 2249, 2447, 2642, 2831, 3012, 3185, 3346, 3495, 3630, 3749, 3853, 3939, 4006, 4055, 4085, 4095, 4085, 4056, 4007, 3940, 3854, 3751, 3632, 3497, 3349, 3187, 3015, 2834, 2645, 2450, 2252, 2051, 1851, 1652, 1457, 1268, 1086,  914,  752,  603,  468,  348,  245,  159,   90,   41,   11,    1,   10,   39,   88,  155,  240,  343,  462,  597,  745,  906, 1078, 1259, 1448, 1642, 1841,
2048, 2249, 2447, 2642, 2831, 3012, 3185, 3346, 3495, 3630, 3749, 3853, 3939, 4006, 4055, 4085, 4095, 4085, 4056, 4007, 3940, 3854, 3751, 3632, 3497, 3349, 3187, 3015, 2834, 2645, 2450, 2252, 2051, 1851, 1652, 1457, 1268, 1086,  914,  752,  603,  468,  348,  245,  159,   90,   41,   11,    1,   10,   39,   88,  155,  240,  343,  462,  597,  745,  906, 1078, 1259, 1448, 1642, 1841,
2048, 2249, 2447, 2642, 2831, 3012, 3185, 3346, 3495, 3630, 3749, 3853, 3939, 4006, 4055, 4085, 4095, 4085, 4056, 4007, 3940, 3854, 3751, 3632, 3497, 3349, 3187, 3015, 2834, 2645, 2450, 2252, 2051, 1851, 1652, 1457, 1268, 1086,  914,  752,  603,  468,  348,  245,  159,   90,   41,   11,    1,   10,   39,   88,  155,  240,  343,  462,  597,  745,  906, 1078, 1259, 1448, 1642, 1841,
2048, 2249, 2447, 2642, 2831, 3012, 3185, 3346, 3495, 3630, 3749, 3853, 3939, 4006, 4055, 4085, 4095, 4085, 4056, 4007, 3940, 3854, 3751, 3632, 3497, 3349, 3187, 3015, 2834, 2645, 2450, 2252, 2051, 1851, 1652, 1457, 1268, 1086,  914,  752,  603,  468,  348,  245,  159,   90,   41,   11,    1,   10,   39,   88,  155,  240,  343,  462,  597,  745,  906, 1078, 1259, 1448, 1642, 1841
};


DMA_Data_t dma_ctrl[NB_OF_DMA_CHAN * 2] __attribute__((aligned(1024)));

void start_dac ()
{
	ARM_RSTCLK->PER_CLOCK |= ARM_PER_CLOCK_DAC;
	ARM_DAC->DAC_CFG = DAC1_ON;
}

void start_dma ()
{
    debug_puts ("Starting form generation\n");
    
    ARM_RSTCLK->PER_CLOCK |= ARM_PER_CLOCK_DMA;
    ARM_DMA->CONFIG |= ARM_DMA_ENABLE;
    ARM_DMA->CHNL_REQ_MASK_SET = ARM_DMA_DISABLE_ALL;	// disable all requests
    ARM_DMA->CHNL_ENABLE_CLR = ARM_DMA_DISABLE_ALL;		// disable all channels
    ARM_DMA->ERR_CLR = ARM_DMA_ERR_CLR;
    ARM_DMA->CTRL_BASE_PTR = (unsigned) dma_ctrl;
    ARM_DMA->CHNL_PRI_ALT_SET = ARM_DMA_SELECT(DMA_CHAN);
    
    dma_ctrl[DMA_CHAN].SOURCE_END_POINTER = (unsigned)dac_data + sizeof(dac_data) - 2;
    dma_ctrl[DMA_CHAN].DEST_END_POINTER = (unsigned)&ARM_DAC->DAC1_DATA;
    dma_ctrl[DMA_CHAN].CONTROL = 
                       ARM_DMA_DST_INC(ARM_DMA_ADDR_NOINC) | 
                       ARM_DMA_DST_SIZE(ARM_DMA_HALFWORD) |
                       ARM_DMA_SRC_INC(ARM_DMA_HALFWORD) | 
                       ARM_DMA_SRC_SIZE(ARM_DMA_HALFWORD) |
                       ARM_DMA_RPOWER(1) |
                       ARM_DMA_TRANSFERS(sizeof(dac_data)/sizeof(dac_data[0])) |
                       ARM_DMA_PINGPONG;

    dma_ctrl[NB_OF_DMA_CHAN + DMA_CHAN].SOURCE_END_POINTER = (unsigned)dac_data + sizeof(dac_data) - 2;
    dma_ctrl[NB_OF_DMA_CHAN + DMA_CHAN].DEST_END_POINTER = (unsigned)&ARM_DAC->DAC1_DATA;
    dma_ctrl[NB_OF_DMA_CHAN + DMA_CHAN].CONTROL = 
                       ARM_DMA_DST_INC(ARM_DMA_ADDR_NOINC) | 
                       ARM_DMA_DST_SIZE(ARM_DMA_HALFWORD) |
                       ARM_DMA_SRC_INC(ARM_DMA_HALFWORD) | 
                       ARM_DMA_SRC_SIZE(ARM_DMA_HALFWORD) |
                       ARM_DMA_RPOWER(1) |
                       ARM_DMA_TRANSFERS(sizeof(dac_data)/sizeof(dac_data[0])) |
                       ARM_DMA_PINGPONG;
                       
    ARM_DMA->CHNL_REQ_MASK_CLR = ARM_DMA_SELECT(DMA_CHAN);
    ARM_DMA->CHNL_ENABLE_SET = ARM_DMA_SELECT(DMA_CHAN);
}

void start_timer1 ()
{
    ARM_RSTCLK->PER_CLOCK |= ARM_PER_CLOCK_TIMER1;
    ARM_RSTCLK->TIM_CLOCK |= ARM_TIM1_CLK_EN;
    ARM_TIMER1->TIM_PSG = KHZ / 1000 - 1;
    ARM_TIMER1->TIM_ARR = 2;
    ARM_TIMER1->TIM_DMA_RE = ARM_TIM_CNT_ARR_EVENT_RE;
    ARM_TIMER1->TIM_CNTRL = ARM_TIM_CNT_EN;
}


void task (void *arg)
{
    start_dma ();
    for (;;) {
        //if (ARM_DMA->CHNL_PRI_ALT_SET & (1 << DMA_CHAN)) { 
            if ((dma_ctrl[DMA_CHAN].CONTROL & ARM_DMA_TRANSFERS(1024)) == 0)
                dma_ctrl[DMA_CHAN].CONTROL = 
                       ARM_DMA_DST_INC(ARM_DMA_ADDR_NOINC) | 
                       ARM_DMA_DST_SIZE(ARM_DMA_HALFWORD) |
                       ARM_DMA_SRC_INC(ARM_DMA_HALFWORD) | 
                       ARM_DMA_SRC_SIZE(ARM_DMA_HALFWORD) |
                       ARM_DMA_RPOWER(1) |
                       ARM_DMA_TRANSFERS(sizeof(dac_data)/sizeof(dac_data[0])) |
                       ARM_DMA_PINGPONG;
        //} else {
            if ((dma_ctrl[NB_OF_DMA_CHAN + DMA_CHAN].CONTROL & ARM_DMA_TRANSFERS(1024)) == 0)
                dma_ctrl[NB_OF_DMA_CHAN + DMA_CHAN].CONTROL = 
                       ARM_DMA_DST_INC(ARM_DMA_ADDR_NOINC) | 
                       ARM_DMA_DST_SIZE(ARM_DMA_HALFWORD) |
                       ARM_DMA_SRC_INC(ARM_DMA_HALFWORD) | 
                       ARM_DMA_SRC_SIZE(ARM_DMA_HALFWORD) |
                       ARM_DMA_RPOWER(1) |
                       ARM_DMA_TRANSFERS(sizeof(dac_data)/sizeof(dac_data[0])) |
                       ARM_DMA_PINGPONG;
        //}
        
        mutex_wait (&timer.lock);
    }
}


void uos_init ()
{
    debug_puts ("\nTesting DAC with DMA\r\n\n");
    
    timer_init (&timer, KHZ, 1);
    
    start_dac ();
    start_timer1 ();
    
    task_create (task, "Hello!", "hello", 1, stack, sizeof (stack));    
}

