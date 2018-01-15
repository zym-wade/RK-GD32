#include"common.h"
#include"timer.h"

/*init the timer*/
void Timex_init(uint32_t timer_periph,uint16_t arr,uint16_t psc)
{	
	timer_deinit(timer_periph);
	timer_parameter_struct timer_struct;
	timer_struct.timer_alignedmode = TIMER_IC_PSC_DIV1;  //对齐模式的选择
	timer_struct.timer_clockrivision = 0; 
	timer_struct.timer_period = arr;
	timer_struct.timer_prescaler = psc;
	timer_init(timer_periph,&timer_struct);
	timer_interrupt_flag_clear(timer_periph,TIMER_INT_UP); /*clear the interrupt flag*/
	timer_interrupt_enable(timer_periph,TIMER_INT_UPIE);
	switch(timer_periph){
		case TIMER13:
			nvic_irq_enable(TIMER13_IRQn,2,3);
			break;
		case TIMER14:
			nvic_irq_enable(TIMER14_IRQn,2,3);
			break;
		case TIMER5:
			nvic_irq_enable(TIMER5_DAC_IRQn,2,3);
			break;
	}
	timer_enable(timer_periph);
}

