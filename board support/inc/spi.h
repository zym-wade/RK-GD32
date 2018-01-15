#ifndef SPI_H
#define SPI_H

#include "gd32f1x0_gpio.h"
#include"common.h"

#ifndef SpiTxSize
#define SpiTxSize  256
#endif

#ifndef SpiRxSize
#define SpiRxSize  256
#endif


extern gd32f1xx_spi_cmd spi_cmd; /*to receive the cmd*/
//static int8_t  DMATX[SpiTxSize];
//static int8_t  DMARX[SpiTxSize];
extern uint8_t IS_CMD;  /*judge the data`type*/
int32_t spi0_init(void);

void Spi_Write_Byte(uint8_t byte);
uint8_t Spi_Read_Byte(void);
uint8_t Spi_Read_Write_Byte(uint8_t byte);
void Spi_Read_Cmd(uint8_t *cmd);
void Spi_Read_Srting(uint8_t *src);
void Spi_Send_Cmd(uint8_t *cmd,uint8_t len);
void Spi_Send_String(uint8_t *cmd);
#endif
