#ifndef TIMER_H
#define TIMER_H
#include<stdint.h>

/*timer prescaler*/
#define TIMER_Prescaler_1ms 48000
#define TIMER_Prescaler_100us 4800
/*init the timer*/
void Timex_init(uint32_t timer_periph,uint16_t arr,uint16_t psc);

#endif
