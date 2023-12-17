#ifndef TIEMR_H
#define TIMER_H

#include <stdint.h>

extern volatile uint32_t timer2_counter;

void timer_init();

#endif