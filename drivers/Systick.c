// Author: Andrew Benner
// Implementation of functionality of driver for SysTick on FRDM-KL25Z

#include <MKL25Z4.h>
#include "systick.h"
#include <stdbool.h>

static _Bool systick_interrupt_has_occurred=0;

#define SYSTICK_PERIOD ( 1000 ) //Desired Period in ms
#define SYSTICK_TOP ( SYS_CLOCK / SYSTICK_PERIOD )

void configure_systick()
{
	SysTick->LOAD = SYSTICK_TOP;
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | 
		SysTick_CTRL_TICKINT_Msk |
		SysTick_CTRL_ENABLE_Msk;
}

_Bool systick_has_fired()
{
	_Bool retval=systick_interrupt_has_occurred;
	systick_interrupt_has_occurred=0;
	return retval;
}

void SysTick_Handler()
{
	systick_interrupt_has_occurred=1;
}