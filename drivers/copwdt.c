// Author: Andrew Benner
// Implementation of functionality of driver for watchdog timer on FRDM-KL25Z

#include <MKL25Z4.h>
#include "copwdt.h"

void configure_copwdt()
{
	SIM->COPC = SIM_COPC_COPT(3) |
		SIM_COPC_COPCLKS(1) |
		SIM_COPC_COPW(0);
}

void feed_the_watchdog()
{
	SIM->SRVCOP = 0X55;
	SIM->SRVCOP = 0XAA;
}
