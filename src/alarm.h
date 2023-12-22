#ifndef ALARM_H
#define ALARM_H

#include <stdint.h>

#include "ds3231.h"

#define ALARM_BUZZER_TIME 5000 // 5000ms(5s)

extern volatile uint32_t alarm_start_time;
extern volatile int alarm_is_added;
extern volatile int alarm_is_on;
extern volatile int alarm_is_start;

void alarm_set_alarm(rtc_t alarm_time);
int alarm_check_alarm();

#endif