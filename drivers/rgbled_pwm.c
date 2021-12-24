// Author: Andrew Benner
// Implementation of functionality of driver for rgb led with pwm and timer implementation on FRDM-KL25Z

#include <stdint.h>
#include <MKL25Z4.h>
#include "rgbled_pwm.h"

static void set_red_led_duty_cycle(uint16_t duty);
static void set_green_led_duty_cycle(uint16_t duty);
static void set_blue_led_duty_cycle(uint16_t duty);

void configure_rgbled()
{
	//configure red led port
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
	PORTB->PCR[18] = PORT_PCR_MUX(3);

	//configure red/green led timer
	SIM->SCGC6 |= SIM_SCGC6_TPM2_MASK;
	TPM2->SC = TPM_SC_PS(0) |
		TPM_SC_CMOD(0) |
		TPM_SC_CPWMS(0);
	TPM2->MOD = 0XFFFF;

	//configure red led channel
	TPM2->CONTROLS[0].CnSC = TPM_CnSC_MSB(1)
		| TPM_CnSC_MSA(0)
		| TPM_CnSC_ELSB(0)
		| TPM_CnSC_ELSA(1);

	//configure green led port
	PORTB->PCR[19] = PORT_PCR_MUX(3);

	//configure green led channel
	TPM2->CONTROLS[1].CnSC = TPM_CnSC_MSB(1)
		| TPM_CnSC_MSA(0)
		| TPM_CnSC_ELSB(0)
		| TPM_CnSC_ELSA(1);

	//configure blue led port
	SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
	PORTD->PCR[1] = PORT_PCR_MUX(4);
	PTD->PDDR |= (1<<1);

	//configure blue led timer
	SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK;
	TPM0->SC = TPM_SC_PS(0) |
		TPM_SC_CMOD(0) |
		TPM_SC_CPWMS(0);
	TPM0->MOD = 0XFFFF;

	//configure blue led channel
	TPM0->CONTROLS[1].CnSC = TPM_CnSC_MSB(1)
		| TPM_CnSC_MSA(0)
		| TPM_CnSC_ELSB(0)
		| TPM_CnSC_ELSA(1);

	turn_off_rgbled();

	TPM0->SC = TPM_SC_CMOD(1);
	TPM2->SC = TPM_SC_CMOD(1);
}
void set_rgbled_color_to(uint16_t red, uint16_t green, uint16_t blue)
{
	set_red_led_duty_cycle(red);
	set_green_led_duty_cycle(green);
	set_blue_led_duty_cycle(blue);
}
void turn_off_rgbled()
{
	set_red_led_duty_cycle(0);
	set_green_led_duty_cycle(0);
	set_blue_led_duty_cycle(0);
}

static void set_red_led_duty_cycle(uint16_t duty)
{
	TPM2->CONTROLS[0].CnV = duty;
}

static void set_green_led_duty_cycle(uint16_t duty)
{
	TPM2->CONTROLS[1].CnV = duty;
}

static void set_blue_led_duty_cycle(uint16_t duty)
{
	TPM0->CONTROLS[1].CnV = duty;
}