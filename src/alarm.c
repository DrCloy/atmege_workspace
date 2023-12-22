#include <avr/io.h>
#include <avr/iom128.h>
#include <stdint.h>

#include "ds3231.h"

volatile rtc_t alarm_time;
volatile uint32_t alarm_start_time;
volatile int alarm_is_added = 0;
volatile int alarm_is_on = 0;
volatile int alarm_is_start = 0;

void alarm_set_alarm(rtc_t alarm_setting_time) {
    alarm_time.hour = alarm_setting_time.hour;
    alarm_time.min = alarm_setting_time.min;
    alarm_is_added = 1;
}

void alarm_on_alarm(int onoff){

}

void alarm_reset_alarm() {
    alarm_is_added = 0;
}

int alarm_check_alarm() {
    rtc_t current_time;
    ds3231_getTime(&current_time);

    if (current_time.hour == alarm_time.hour && current_time.min == alarm_time.min) return 1;
    else return 0;
}