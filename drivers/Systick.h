// Author: Andrew Benner
// Function declarations of driver for SysTick on FRDM-KL25Z

#ifndef SYSTICK_H
#define SYSTICK_H
#include <stdbool.h>

void configure_systick();
_Bool systick_has_fired();

#endif
