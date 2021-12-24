// Author: Andrew Benner
// Function declarations of driver for sw1 on FRDM-KL25Z

#ifndef SW1_H
#define SW1_H
#include <stdbool.h>

_Bool sw1_is_pressed();
_Bool sw1_is_not_pressed();
void configure_sw1();

#endif
