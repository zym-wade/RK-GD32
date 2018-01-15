#ifndef USART_H
#define USART_H
#include<stdint.h>
#include"fifo.h"

#define USART_FLOW_CONTROL 0   /*usart2 use flow control or not*/ 
#define USART1_OPEN 0     /*the switch to open the usart1 or not*/
#define USART_DEBUG 0     /*debug for usart*/

#ifndef Usart_Baudval
#define Usart_Baudval  115200  /*usart`s baudrate*/
#endif

#define USART_DMA_DEBUG 0

extern Queue Usart0_Rx_Fifo;
extern Queue Usart1_Rx_Fifo;

void usart_init(void);
uint32_t usartx_init(uint32_t usart_periph);
void usart_gpio_init(void);
uint32_t USART_Send_BYTE(uint32_t usart_periph,int ch);
void USART_Send_Array(uint32_t usart_periph,uint8_t array[],uint32_t len);
void USART_Send_Str(uint32_t usart_periph,uint8_t *str);
void Usart_Rx_Packet(Queue* q,uint32_t len,uint8_t portid);/*packet the data and add to the spi_list*/


#endif
