// Author: Andrew Benner
/* This program uses the accelerometer to allow the user to choose different colors on the board
per plane. All 3 colors are added at the end to create a new color. */

#include "rgbled_pwm.h"
#include "i2c0_irq.h"
#include "copwdt.h"
#include "sw1.h"
#include "Systick.h"
#include <stdint.h>
#include <stdbool.h>

#define DEBOUNCE_TIME_MS 5
#define LONG_PRESS_TIME_MS 1500
#define ACCEL_I2C_ADDR 0x1d
#define I2C_BUFSIZE 7
volatile uint8_t i2c_data[I2C_BUFSIZE];

typedef enum{NONE,SHORTP,LONGP}press_t;
press_t switch_check_fsm();

void accelerometer_fsm();
void color_calculator(_Bool Z_Color, _Bool X_Color, _Bool Y_Color);

uint16_t R_Duty = 0x0000;
uint16_t G_Duty = 0x0000;
uint16_t B_Duty = 0x0000;

void main()
{
	asm("CPSID i");
	configure_rgbled();
	configure_sw1();
	configure_i2c0();
	configure_copwdt();
	configure_systick();
	asm("CPSIE i");

	_Bool Z_Color;
	_Bool X_Color;
	_Bool Y_Color;

	while(1)
	{
		asm("WFI"); //Wait for SysTick IRQ
		if(!systick_has_fired())
		{
			continue;
		}

		accelerometer_fsm();	
		press_t pressed = switch_check_fsm();

		static enum {ST_IDLE, ST_Z, ST_X, ST_Y, ST_FINAL} state = ST_IDLE;
		switch(state)
		{
		case ST_IDLE:
			set_rgbled_color_to(0,0,0);
			if(pressed == SHORTP)
			{
				state = ST_Z;
			}
			break;
		case ST_Z:
			if(((i2c_data[3])<<8) <	0x8000)
			{
				set_rgbled_color_to((i2c_data[3])<<8,0,0);
				if(pressed == SHORTP)
				{
					Z_Color = true; //red
					state = ST_X;
				}
			}
			else
			{
				set_rgbled_color_to(0,(i2c_data[3])<<8,0);
				if(pressed == SHORTP)
				{
					Z_Color = false; //green
					state = ST_X;
				}
			}
			break;
		case ST_X:
			if(((i2c_data[1])<<8) <	0x8000)
			{
				set_rgbled_color_to(0,0,(i2c_data[1])<<8);
				if(pressed == SHORTP)
				{
					X_Color = true; //blue
					state = ST_Y;
				}
			}
			else
			{
				set_rgbled_color_to(((i2c_data[1])<<8)/2,((i2c_data[1])<<8)/2,0);
				if(pressed == SHORTP)
				{
					X_Color = false; //yellow
					state = ST_Y;
				}
			}
			break;
		case ST_Y:
			if(((i2c_data[2])<<8) <	0x8000)
			{
				set_rgbled_color_to(((i2c_data[2])<<8)/2,0,((i2c_data[2])<<8)/2);
				if(pressed == SHORTP)
				{
					Y_Color = true; //purple
					state = ST_FINAL;
				}
			}
			else
			{
				set_rgbled_color_to(0,((i2c_data[2])<<8)/2,((i2c_data[2])<<8)/2);
				if(pressed == SHORTP)
				{
					Y_Color = false; //cyan
					state = ST_FINAL;
				}
			}
			break;
		case ST_FINAL:
			color_calculator(Z_Color, X_Color, Y_Color);
			set_rgbled_color_to(R_Duty, G_Duty, B_Duty);
			if(pressed == SHORTP)
			{
				state = ST_IDLE;
			}
			break;
		}

		feed_the_watchdog();

	}
	return;
}

void accelerometer_fsm()
{
	static enum {ST_START, ST_WAIT_WHOAMI, ST_WHOAMI_SUCCESS, ST_WHOAMI_FAIL, ST_CONFIG_WAIT, ST_READ_ACCEL_DATA} state=ST_START;
	switch(state)
	{
	case ST_START: 
		i2c0_read_bytes(ACCEL_I2C_ADDR <<1, 0x0d, 1, i2c_data);
		state=ST_WAIT_WHOAMI;
		break;
	case ST_WAIT_WHOAMI:
		if(i2c0_last_transaction_complete())
		{
			if(i2c0_last_transaction_had_error())
			{
				state=ST_START;
			}
			if(i2c_data[0] == 0x1A)
			{
				state=ST_WHOAMI_SUCCESS;
			}
			else
			{
				state=ST_WHOAMI_FAIL;
			}
		}
		break;
	case ST_WHOAMI_SUCCESS:
		i2c0_write_byte(ACCEL_I2C_ADDR <<1, 0x2a, 0x1f);
		state=ST_CONFIG_WAIT;
		break;
	case ST_WHOAMI_FAIL:
		break;
	case ST_CONFIG_WAIT:
		if(i2c0_last_transaction_complete())
		{
			if(i2c0_last_transaction_had_error())
			{
				state=ST_WHOAMI_SUCCESS;
			}
			else
			{
				state=ST_READ_ACCEL_DATA;
				i2c0_read_bytes(ACCEL_I2C_ADDR <<1, 0x0, 4, i2c_data);
			}
		}
		break;	
	case ST_READ_ACCEL_DATA:
		if(i2c0_last_transaction_complete())
		{
			i2c0_read_bytes(ACCEL_I2C_ADDR <<1, 0x0, 4, i2c_data);
		}
		break;
	}
	
}

void color_calculator(_Bool Z_Color, _Bool X_Color, _Bool Y_Color)
{
	if((Z_Color == false)&&(X_Color == false)&&(Y_Color == false))
	{
		R_Duty = 0x2aaa;
		G_Duty = 0xaaaa;
		B_Duty = 0x2aaa;
	}
	else if((Z_Color == true)&&(X_Color == true)&&(Y_Color == true))
	{
		R_Duty = 0x8000;
		G_Duty = 0;
		B_Duty = 0x8000;
	}
	else if((Z_Color == true)&&(X_Color == false)&&(Y_Color == false))
	{
		R_Duty = 0x8000;
		G_Duty = 0x5555;
		B_Duty = 0x2aaa;
	}
	else if((Z_Color == false)&&(X_Color == true)&&(Y_Color == false))
	{
		R_Duty = 0;
		G_Duty = 0x8000;
		B_Duty = 0x8000;
	}
	else if((Z_Color == false)&&(X_Color == false)&&(Y_Color == true))
	{
		R_Duty = 0x5555;
		G_Duty = 0x8000;
		B_Duty = 0x2aaa;
	}
	else if((Z_Color == true)&&(X_Color == true)&&(Y_Color == false))
	{
		R_Duty = 0x5555;
		G_Duty = 0x2aaa;
		B_Duty = 0x8000;
	}
	else if((Z_Color == true)&&(X_Color == false)&&(Y_Color == true))
	{
		R_Duty = 0x8000;
		G_Duty = 0x5555;
		B_Duty = 0x2aaa;
	}
	else if((Z_Color == false)&&(X_Color == true)&&(Y_Color == true))
	{
		R_Duty = 0x2aaa;
		G_Duty = 0x5555;
		B_Duty = 0x8000;
	}
}

press_t switch_check_fsm()
{
	static uint16_t switch_time=0;
	static enum {ST_NO_PRESS, ST_DEBOUNCE_PRESS, ST_PRESS,
		ST_LONG_PRESS, ST_DEBOUNCE_RELEASE} sw_state=ST_NO_PRESS;

	switch_time++;

	switch(sw_state){
	 default:
	 case ST_NO_PRESS:
		 if( sw1_is_pressed() )
		 {
			 switch_time = 0;
			 sw_state = ST_DEBOUNCE_PRESS;
		 }
		 break;
	 case ST_DEBOUNCE_PRESS:
		 if( sw1_is_not_pressed() )
			 sw_state = ST_NO_PRESS;
		 else if( switch_time >= DEBOUNCE_TIME_MS )
			 sw_state = ST_PRESS;
		 break;
	 case ST_PRESS:
		 if( sw1_is_not_pressed() )
		 {
			 switch_time=0;
			 sw_state = ST_DEBOUNCE_RELEASE;
			 return SHORTP;
		 }
		 else if( switch_time >= LONG_PRESS_TIME_MS )
			 sw_state = ST_LONG_PRESS;
		 break;
	 case ST_LONG_PRESS :
		 if( sw1_is_not_pressed() )
		 {
			 switch_time=0;
			 sw_state = ST_DEBOUNCE_RELEASE;
			 return LONGP;
		 }
		 break;
	 case ST_DEBOUNCE_RELEASE:
		 if( switch_time >= DEBOUNCE_TIME_MS )
			 sw_state = ST_NO_PRESS;
		 break;
	}
	return NONE;
}