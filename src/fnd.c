#include <avr/io.h>
#include <avr/iom128.h>
#include <stdint.h>
#include <util/delay.h>

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

void fnd_print_time(uint8_t hour, uint8_t min) {
    uint8_t fnd_value[4];
    fnd_value[0] = hour / 10;
    fnd_value[1] = hour % 10;
    fnd_value[2] = min / 10;
    fnd_value[3] = min % 10;

    for (int i = 0; i < 4; i++) {
        FND_DIGIT_DATA = fnd_number[fnd_value[i]] | ((i == 1) ? FND_DP : 0);
        FND_SELECT_DATA = 1 << (3 - i);
        _delay_ms(20);
    }
}