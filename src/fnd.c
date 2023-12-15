#include <avr/io.h>
#include <avr/iom128.h>
#include <stdint.h>
#include <util/delay.h>
#include "ds3231.h"

#define F_CPU 16000000UL
#define FND_DIGIT        DDRC
#define FND_DIGIT_DATA   PORTC
#define FND_SELECT       DDRG
#define FND_SELECT_DATA  PORTG
#define FND_SEL_0        PG0
#define FND_SEL_1        PG1
#define FND_SEL_2        PG2
#define FND_SEL_3        PG3
#define FND_DP           (1 << 7)

volatile uint8_t fnd_number[10] = {
    0x3f, 0x06, 0x5b, 0x4f, 0x66, 
    0x6d, 0x7d, 0x07, 0x7f, 0x6f
};

void fnd_init() {
    FND_DIGIT = 0xff;
    FND_SELECT = ((1 << FND_SEL_0) | (1 << FND_SEL_1) | (1 << FND_SEL_2) | (1 << FND_SEL_3));
}

void fnd_print_function(int index) {
    uint8_t index_value[4];
    index_value[0] = 0x71; // F
    index_value[1] = 0x3e; // U
    index_value[2] = (index / 10);
    index_value[3] = (index % 10);

    for (int i = 0; i < 4; i++) {
        FND_DIGIT_DATA = fnd_number[index_value[i]] | ((i == 1) ? FND_DP : 0);
        FND_SELECT_DATA = 1 << (3 - i);
        _delay_ms(20);
    }
}

void fnd_print_time() {
    rtc_t current_time;
    ds3231_getTime(&current_time);

    uint8_t fnd_value[4];
    fnd_value[0] = current_time.hour / 10;
    fnd_value[1] = current_time.hour % 10;
    fnd_value[2] = current_time.min / 10;
    fnd_value[3] = current_time.min % 10;

    for (int i = 0; i < 4; i++) {
        FND_DIGIT_DATA = fnd_number[fnd_value[i]] | ((i == 1) ? FND_DP : 0);
        FND_SELECT_DATA = 1 << (3 - i);
        _delay_ms(20);
    }
}