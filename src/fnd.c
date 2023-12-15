#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/iom128.h>
#include <stdint.h>
#include <util/delay.h>

#include "fnd.h"
#include "switch.h"
#include "ds3231.h"

#define FND_DIGIT        DDRC
#define FND_DIGIT_DATA   PORTC
#define FND_SELECT       DDRG
#define FND_SELECT_DATA  PORTG
#define FND_SEL_0        PG0
#define FND_SEL_1        PG1
#define FND_SEL_2        PG2
#define FND_SEL_3        PG3
#define FND_DP           PC7

volatile uint8_t fnd_number[10] = {
    0x3f, 0x06, 0x5b, 0x4f, 0x66, 
    0x6d, 0x7d, 0x07, 0x7f, 0x6f
};

void fnd_init() {
    FND_DIGIT = 0xff;
    FND_SELECT = ((1 << FND_SEL_0) | (1 << FND_SEL_1) | (1 << FND_SEL_2) | (1 << FND_SEL_3));
}

void fnd_print(uint8_t *value, int dp_index) {
    for (int i = 0; i < 4; i++) {
        FND_DIGIT_DATA = value[i] | ((i == dp_index) ? (1 << FND_DP) : 0);
        FND_SELECT_DATA = 1 << (3 - i);
        _delay_ms(1);
    }
}

void fnd_print_function(int index) {
    uint8_t index_value[4];
    index_value[0] = 0x71; // F
    index_value[1] = 0x3e; // U
    index_value[2] = fnd_number[(index / 10)];
    index_value[3] = fnd_number[(index % 10)];

    fnd_print(index_value, 1);
}

void fnd_print_time() {
    rtc_t current_time;
    while (!switch_read()) {
        ds3231_getTime(&current_time);

        uint8_t fnd_value[4];
        fnd_value[0] = fnd_number[current_time.hour / 10];
        fnd_value[1] = fnd_number[current_time.hour % 10];
        fnd_value[2] = fnd_number[current_time.min / 10];
        fnd_value[3] = fnd_number[current_time.min % 10];

        fnd_print(fnd_value, 1);
    }
}

void fnd_print_setting_time(rtc_t setting_time, int hour_min) {
    uint8_t fnd_value[4];
    fnd_value[0] = fnd_number[setting_time.hour / 10];
    fnd_value[1] = fnd_number[setting_time.hour % 10];
    fnd_value[2] = fnd_number[setting_time.min / 10];
    fnd_value[3] = fnd_number[setting_time.min % 10];

    fnd_print(fnd_value, 1);

    if (hour_min == SETTING_HOUR) {
        fnd_value[0] = 0x00;
        fnd_value[1] = 0x00;

        fnd_print(fnd_value, 1);
    } else if (hour_min == SETTING_MIN) {
        fnd_value[2] = 0x00;
        fnd_value[3] = 0x00;

        fnd_print(fnd_value, 1);
    }
}