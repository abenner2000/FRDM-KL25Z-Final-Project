// Author: Andrew Benner
// Function declarations of driver for rgb led with pwm and timer implementation on FRDM-KL25Z

#ifndef RGBLED_PWM_H
#define RGBLED_PWM_H
#include <stdint.h>

void configure_rgbled();
void set_rgbled_color_to(uint16_t red, uint16_t green, uint16_t blue);
void turn_off_rgbled();

#endif
