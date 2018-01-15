#include"spi.h"
#include<stdlib.h>
#include"dma.h"
#include"common.h"
#include<string.h>
/*attention:in keil,the define of SPI0 is equal to datasheet`s SPI1*/


gd32f1xx_spi_cmd spi_cmd;

/**
  \fn          int32_t spi0_init (void)
  \brief       Initialize Spi
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t spi0_init()
{
	spi_parameter_struct spi_struct; /*spi_struct: SPI parameter initialization stuct*/
	
	/*GPIO configure*/
	gpio_deinit(GPIOA);
	gpio_mode_set(GPIOA,GPIO_MODE_AF,GPIO_PUPD_PULLUP,GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
  /*Set to standby mode :CSS CLK MISO MOSI*/
	gpio_af_set(GPIOA,GPIO_AF_0,GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
	/*slave output pin:MISO*/
	gpio_output_options_set(GPIOA,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_6);
	
	/*Initialize DMA1:spi receive*/
	Peripheral_DMA_Configuration((uint32_t)&SPI_DATA(SPI0),DMA_PERIPHERA_TO_MEMORY,DMA_CH1);
	/*Initialize DMA2:spi send*/
	Peripheral_DMA_Configuration((uint32_t)&SPI_DATA(SPI0),DMA_MEMORY_TO_PERIPHERA,DMA_CH2);
	spi_dma_enable(SPI0,SPI_DMA_SEND);   /*enable the spi DMA */
	spi_dma_enable(SPI0,SPI_DMA_RECEIVE);
	
	/*SPI configuire*/
	spi_struct.spi_devicemode = SPI_SLAVE;
	spi_struct.spi_transmode = SPI_TRANSMODE_FULLDUPLEX;  
	spi_struct.spi_framesize = SPI_FRAMESIZE_8BIT;
	spi_struct.spi_nss = SPI_NSS_HARD;     //hardware to select
	spi_struct.spi_endian = SPI_ENDIAN_MSB;   //same with px3se  little endian
	spi_struct.spi_ck_pl_ph = SPI_CK_PL_LOW_PH_2EDGE;   /*SCPOL = 0  SCPH = 1*/
	spi_struct.spi_psc = SPI_PSC_8;       //the clk is provided by the host 
	spi_init(SPI0,&spi_struct);
	/*crc function turn off*/
	spi_crc_off(SPI0);

	nvic_irq_enable(DMA_Channel1_2_IRQn,1,1);   /*enable DMA irq interrupt*/
	nvic_irq_enable(SPI0_IRQn,2,2); /*enable SPI irq receive interrupt*/
#if Mode_Work
	spi_i2s_interrupt_enable(SPI0,SPI_I2S_INT_RBNE);  /*spi interrupt*/ 
#else	
  /*start to receive the cmd*/
	DMA_Transmit((uint32_t)&spi_cmd,gd32f1xx_spi_cmd_len,DMA_CH1);
	/*enable spi0*/
#endif	
	spi_enable(SPI0);  
	return 0;
}


/**
  \fn          void SPI0_IRQHandler (void)
  \brief       Spi  receive irq
*/

uint8_t spi_flag=0;
extern uint8_t updata_receive_cmd_buf[gd32f1xx_spi_cmd_len+1];
extern uint8_t firm_updata_disen;
void SPI0_IRQHandler()
{
	/*judge the receive interrupt*/
	if(spi_i2s_flag_get(SPI0,SPI_FLAG_RBNE) == SET){ 
		Spi_Read_Cmd(updata_receive_cmd_buf);
		spi_i2s_interrupt_disable(SPI0,SPI_I2S_INT_RBNE);  /*close the int in case when transfer would into the int*/
		firm_updata_disen=0;  /*demutex the main while to deal the cmd*/
	}
	
}

uint8_t IS_CMD=1;
/**
  \fn          void DMA_Channel1_2_IRQHandler (void)
  \brief       SPI DMA RECEIVE and SEND IRQ
*/
spi_data *data;
uint8_t send_reverse=0;
void DMA_Channel1_2_IRQHandler()
{
	/*DMA1  receive done*/
	if( dma_interrupt_flag_get(DMA_CH1,DMA_CHXCTL_FTFIF) == SET ){
		dma_interrupt_flag_clear(DMA_CH1,DMA_CHXCTL_FTFIF);   /*clear the transfer complete flag*/
		dma_interrupt_flag_clear(DMA_CH1,DMA_INTF_ANYIF);  /*clear the global interrupt flag of channel*/		
		if( IS_CMD ){
			/*start to receive the cmd*/
	    data=(spi_data*)malloc( spi_data_cmd_len+spi_cmd.data_len+1 );
			data->next=NULL;	
			spi_dma_receive_en = 0; /*until cmd+data transfer over*/
			/*copy the receive cmd to the list`data*/
			memcpy(&data->cmd,(void *)&spi_cmd,gd32f1xx_spi_cmd_len);
			DMA_Transmit((uint32_t)(data->buff),spi_cmd.data_len,DMA_CH1);
		}else{
			spi_receive_data_tail->next=data; /*add to the tail of list*/
	    spi_receive_data_tail = data;
			//printf("%s %d\r\n",spi_receive_data_tail->buff,data->cmd.data_len);
			DMA_Transmit((uint32_t)&spi_cmd,gd32f1xx_spi_cmd_len,DMA_CH1);
			spi_dma_receive_en = 1;
		}
		IS_CMD = !IS_CMD;   /*flip the flag everytime*/
	}
	/*DMA2  send done*/
	if( dma_interrupt_flag_get(DMA_CH2,DMA_CHXCTL_FTFIF) == SET ){
		while( spi_i2s_flag_get(SPI0,SPI_FLAG_TRANS)==SET );  /*until TBE=1,trans=0 need to test*/
		dma_interrupt_flag_clear(DMA_CH2,DMA_CHXCTL_FTFIF);   /*clear the transfer complete flag of channel2*/
		dma_interrupt_flag_clear(DMA_CH2,DMA_INTF_ANYIF);
		if( send_reverse==0 ){
			DMA_Transmit((uint32_t)spi_send_data_head->next->buff,spi_send_data_head->next->cmd.data_len,DMA_CH2);
		}else{
			/*delete the data from the list */
			spi_data *temp=spi_send_data_head->next;
			spi_send_data_head->next=spi_send_data_head->next->next;
			if( spi_send_data_head->next==NULL ){
				spi_send_data_tail = spi_send_data_head;
			}
			free(temp);
			temp=NULL;
			spi_dma_send_en=1;
			Spi_Read_Byte();   /*read the remain dumy data to make sure the dma-receive correct*/
			spi_dma_enable(SPI0,SPI_DMA_RECEIVE); /*receive a full packet then enable the transfer*/
		}
		send_reverse=!send_reverse;
	}
}


void Spi_Read_Srting(uint8_t *src)
{
	while(1){
		*src = Spi_Read_Byte();
		if(*src == 0){
			spi_i2s_interrupt_enable(SPI0,SPI_I2S_INT_RBNE);  
			break;
		}
		src++;
	}
}	

/**
  \fn          uint8_t Spi_Read_Write_Byte(uint8_t byte)
  \brief       write and read a byte through Spi
  \param[in]   byte: data to send
  \returns     uint8_t
   - the byte  that feedback  
*/
uint8_t Spi_Read_Write_Byte(uint8_t byte)
{
	while( spi_i2s_flag_get(SPI0,SPI_FLAG_TBE)==RESET );
	spi_i2s_data_send(SPI0,byte);
	while( spi_i2s_flag_get(SPI0,SPI_FLAG_RBNE)==RESET );
	return spi_i2s_data_receive(SPI0);  /*data receive*/
}

/**
  \fn          uint8_t Spi_Read_Byte()
  \brief       read a buye through Spi
  \returns     uint8_t
   - the read byte
*/
uint8_t Spi_Read_Byte()
{
	while( spi_i2s_flag_get(SPI0,SPI_FLAG_RBNE)==RESET );
	return spi_i2s_data_receive(SPI0);
}


void Spi_Read_Cmd(uint8_t *cmd)
{
	uint16_t i=0;
	while( i<gd32f1xx_spi_cmd_len ){
		*cmd++ = Spi_Read_Byte();
		i++;
	}
}
/**
  \fn          void Spi_Write_Byte(uint8_t byte)
  \brief       write a byte
	\param[in]   byte: data to write
  \returns     NULL
*/
void Spi_Write_Byte(uint8_t byte)
{
	while( spi_i2s_flag_get(SPI0,SPI_FLAG_TBE)==RESET );
	spi_i2s_data_send(SPI0,byte);
}

/**
  \fn          void Spi_Send_Cmd(uint8_t *cmd,uint8_t len)
  \brief       send the cmd 
	\param[in]   cmd: the address of cmd
	\param[in]   len: the length of cmd
  \returns     NULL
*/
void Spi_Send_Cmd(uint8_t *cmd,uint8_t len)
{
	uint8_t i=0;
	while(i<len){
		Spi_Read_Write_Byte(cmd[i]);
		i++;
	}
}

/**
  \fn          void Spi_Send_String(uint8_t *cmd)
  \brief       send the String 
	\param[in]   cmd: the address of string
  \returns     NULL
*/
void Spi_Send_String(uint8_t *cmd)
{
	while(*cmd){
		Spi_Read_Write_Byte(*cmd++);
	}
	Spi_Read_Write_Byte(*cmd);
}
//extern spi_data *spi_receive_data_head;
/**
  \fn      void TIMER14_IRQHandler()
  \brief   check whether the list has data,if any,active the spi DMA send
	packet-->copy data-->spi cmd+data
*/
void TIMER14_IRQHandler()
{
	/*detection usart send*/
	if( spi_receive_data_head->next!=NULL ){
		USART_SEND_EN=1;
	}
	//printf("i am time14\r\n");
	timer_interrupt_flag_clear(TIMER14,TIMER_INT_UP);
}

void TIMER5_DAC_IRQHandler()
{
	//printf("i am time5\r\n");
	timer_interrupt_flag_clear(TIMER5,TIMER_INT_UP);
}
