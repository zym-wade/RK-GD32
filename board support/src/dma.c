#include"common.h"
#include"dma.h"

/**
  \fn          void DMA_Transmit(uint32_t memory_addr,uint32_t number,uint8_t dir)
  \brief       DMA transmit:send or receive
  \param[in]  memory_addr
  \param[in]  number:number of transmit
  \param[in]  dir:
		\arg:DMA_PERIPHERA_TO_MEMORY
		\arg:DMA_MEMORY_TO_PERIPHERA
  \returns    NULL
*/
void DMA_Transmit(uint32_t memory_addr,uint32_t number,dma_channel_enum channelx)
{
	dma_channel_disable(channelx);       /*close the channel and set the arg*/
	dma_memory_address_config(channelx,memory_addr);
	dma_transfer_number_config(channelx,number);
	dma_channel_enable(channelx);         /*start to transmit*/
}


/**
  \fn         void Peripheral_DMA_Configuration(uint32_t periph_addr,uint8_t direction,dma_channel_enum channelx)
  \brief      configure the DMA transmit:send or receive
  \param[in]  periph_addr
  \param[in]  direction:
		\arg:DMA_PERIPHERA_TO_MEMORY
		\arg:DMA_MEMORY_TO_PERIPHERA
  \param[in]  channelx:
    \arg:DMA_CH0 = 0, 
    \arg:DMA_CH1,     
    \arg:DMA_CH2,    
    \arg:DMA_CH3,   
    \arg:DMA_CH4,     
    \arg:DMA_CH5,    
    \arg:DMA_CH6   
  \returns    NULL
*/
void Peripheral_DMA_Configuration(uint32_t periph_addr,uint8_t direction,dma_channel_enum channelx)
{
	dma_parameter_struct dma_struct; /* DMA initialize struct */
  dma_struct.periph_addr = periph_addr;
	dma_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;  /*8 bit cache*/
	dma_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
	dma_struct.memory_addr = (uint32_t)0;    /*need configure*/
	dma_struct.memory_width = DMA_PERIPHERAL_WIDTH_8BIT;
	dma_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
	dma_struct.direction = direction;
	dma_struct.number = 0;                  /*need configure*/
	dma_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
	dma_init(channelx,dma_struct);          /*DMA1  receive  init*/
	dma_interrupt_enable(channelx,DMA_CHXCTL_FTFIE); /*enable channel transfer complete interrupt*/
}
