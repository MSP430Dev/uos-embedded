/*
 * Пример работы с DMA. DMA используется в режиме передачи память-память.
 */
#include <runtime/lib.h>
#include <kernel/uos.h>
#include <stream/stream.h>

ARRAY (stack, 1000);

DMA_Data_t dma_prim __attribute__((aligned(1024)));

uint32_t src_buf[256];
uint16_t dst_buf[256];

void task (void *arg)
{
    int i;
    int ecnt;
    
    ARM_RSTCLK->PER_CLOCK |= ARM_PER_CLOCK_DMA;
    ARM_DMA->CONFIG |= ARM_DMA_ENABLE;
    ARM_DMA->CHNL_REQ_MASK_SET = ARM_DMA_DISABLE_ALL;	// disable all requests
    ARM_DMA->CHNL_ENABLE_CLR = ARM_DMA_DISABLE_ALL;		// disable all channels
    ARM_DMA->ERR_CLR = ARM_DMA_ERR_CLR;
    ARM_DMA->CTRL_BASE_PTR = (unsigned) &dma_prim;
    ARM_DMA->CHNL_PRI_ALT_CLR = ARM_DMA_SELECT(0);
    ARM_DMA->CHNL_REQ_MASK_CLR = ARM_DMA_SELECT(0);
    ARM_DMA->CHNL_USEBURST_CLR = ARM_DMA_SELECT(0);
       
    // Будем передавать "с упаковкой" 32->16, т.е. из каждого
    // 32-разрядного целого источника будут переданы только младшие
    // 16 бит
    debug_puts ("Checking transfer 32->16...");
    
    for (i = 0; i < sizeof(src_buf)/sizeof(src_buf[0]); ++i)
        src_buf[i] = (i << 24) | (i << 16) | (i << 8) | i;
    
    dma_prim.SOURCE_END_POINTER = (unsigned)src_buf + sizeof(src_buf);
    dma_prim.DEST_END_POINTER = (unsigned)dst_buf + sizeof(dst_buf);
    dma_prim.CONTROL = ARM_DMA_DST_INC(ARM_DMA_HALFWORD) | 
                       ARM_DMA_DST_SIZE(ARM_DMA_HALFWORD) |
                       ARM_DMA_SRC_INC(ARM_DMA_WORD) | 
                       ARM_DMA_SRC_SIZE(ARM_DMA_HALFWORD) |
                       ARM_DMA_RPOWER(1) |
                       ARM_DMA_TRANSFERS(sizeof(src_buf)/sizeof(src_buf[0])) |
                       ARM_DMA_AUTOREQ;
                       
    ARM_DMA->CHNL_ENABLE_SET = ARM_DMA_SELECT(0);
    ARM_DMA->CHNL_SW_REQUEST = ARM_DMA_SELECT(0);
         
    while ((dma_prim.CONTROL & (ARM_DMA_TRANSFERS(1024) | ARM_DMA_AUTOREQ)) != 0);
    
    ecnt = 0;
    for (i = 0; i < sizeof(dst_buf)/sizeof(dst_buf[0]); ++i)
        if (dst_buf[i] != ((i << 8) | i))
            ecnt++;
    
    if (ecnt == 0)
        debug_puts ("OK!\n");
    else debug_puts ("FAILED\n");
    
    // А теперь будем передавать "с распаковкой" 16->32, т.е. каждые
    // 16-разрядные целые из источника будут положены с шагом 32-бита
    // в приёмнике  
    debug_puts ("Checking transfer 16->32...");
    
    ARM_DMA->CHNL_ENABLE_CLR = ARM_DMA_SELECT(0);
    
    memset (src_buf, 0, sizeof(src_buf));
    
    dma_prim.SOURCE_END_POINTER = (unsigned)dst_buf + sizeof(dst_buf);
    dma_prim.DEST_END_POINTER = (unsigned)src_buf + sizeof(src_buf);
    dma_prim.CONTROL = ARM_DMA_DST_INC(ARM_DMA_WORD) | 
                       ARM_DMA_DST_SIZE(ARM_DMA_HALFWORD) |
                       ARM_DMA_SRC_INC(ARM_DMA_HALFWORD) | 
                       ARM_DMA_SRC_SIZE(ARM_DMA_HALFWORD) |
                       ARM_DMA_RPOWER(1) |
                       ARM_DMA_TRANSFERS(sizeof(src_buf)/sizeof(src_buf[0])) |
                       ARM_DMA_AUTOREQ;
                       
    ARM_DMA->CHNL_ENABLE_SET = ARM_DMA_SELECT(0);
    ARM_DMA->CHNL_SW_REQUEST = ARM_DMA_SELECT(0);
         
    while ((dma_prim.CONTROL & (ARM_DMA_TRANSFERS(1024) | ARM_DMA_AUTOREQ)) != 0);
    
    ecnt = 0;
    for (i = 0; i < sizeof(src_buf)/sizeof(src_buf[0]); ++i)
        if (src_buf[i] != ((i << 8) | i))
            ecnt++;
    
    if (ecnt == 0)
        debug_puts ("OK!\n");
    else debug_puts ("FAILED\n");
    
    
    for (;;);
}

void uos_init (void)
{
    debug_puts ("\nTesting DMA\r\n\n");
    task_create (task, "Hello!", "hello", 1, stack, sizeof (stack));
}
