#ifndef TIEMR_H
#define TIMER_H

#include <stdint.h>

volatile static uint32_t timer0_counter = 0;

void timer_init();

#endif