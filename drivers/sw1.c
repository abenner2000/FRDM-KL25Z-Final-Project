// Author: Andrew Benner
// Implementation of functionality of driver for sw1 on FRDM-KL25Z

#include <MKL25Z4.h>
#include "sw1.h"
#include <stdbool.h>

#define SW1_LOCATION 20

void configure_sw1()
{
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
	PORTA->PCR[SW1_LOCATION] = 
		PORT_PCR_PE(1) |
		PORT_PCR_PS(1) |
		PORT_PCR_PFE(0) |
		PORT_PCR_DSE(0) |
		PORT_PCR_SRE(0) |
		PORT_PCR_MUX(1) |
		PORT_PCR_IRQC(0) |
		PORT_PCR_ISF(1);
	PTA->PDDR &= ~(1<<SW1_LOCATION);
}

_Bool sw1_is_pressed()
{
	return !(PTA->PDIR & (1<<SW1_LOCATION));
}


_Bool sw1_is_not_pressed()
{
	return !sw1_is_pressed();
}
