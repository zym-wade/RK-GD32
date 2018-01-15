#ifndef DMA_H
#define DMA_H
#include<stdint.h>
#include"gd32f1x0_dma.h"
/*configure DMA transmit:send or receive*/
void DMA_Transmit(uint32_t memory_addr,uint32_t number,dma_channel_enum channelx);
/*configure the DMA transmit:send or receive*/
void Peripheral_DMA_Configuration(uint32_t periph_addr,uint8_t direction,dma_channel_enum channelx);

#endif
