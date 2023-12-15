#include <avr/io.h>
#include <avr/iom128.h>
#include <stdint.h>
#include <avr/interrupt.h>

#define F_CPU 16000000UL
// For RGB mode
#define RGB_NONE 0
#define RGB_R 1
#define RGB_G 2
#define RGB_B 4

volatile static uint8_t timer_counter = 0;
volatile static uint8_t red = 0xff;
volatile static uint8_t green = 0x00;
volatile static uint8_t blue = 0x00;
volatile static uint8_t rgb_mode = RGB_R;

SIGNAL(TIMER0_COMP_vect) {
    // RGB 값을 50ms에 1씩 바꿔준다. 
    timer_counter += 1;
    if (timer_counter == 25) {
        timer_counter = 0;
        switch (rgb_mode)
        {
        case RGB_R:
            green += 1;
            if (green == 0xff) {
                rgb_mode = RGB_R | RGB_G;
            }
            break;
        case RGB_R | RGB_G:
            red -= 1;
            if (red == 0x00) {
                rgb_mode = RGB_G;
            }
            break;
        case RGB_G:
            blue += 1;
            if (blue == 0xff) {
                rgb_mode = RGB_G | RGB_B;
            }
            break;
        case RGB_G | RGB_B:
            green -= 1;
            if (green == 0x00) {
                rgb_mode = RGB_B;
            }
            break;
        case RGB_B:
            red += 1;
            if (red == 0xff) {
                rgb_mode = RGB_B | RGB_R;
            }
            break;
        case RGB_B | RGB_R:
            blue -= 1;
            if (blue == 0x00) {
                rgb_mode = RGB_R;
            }
            break;
        default:
            break;
        }
        OCR1A = red;
        OCR1B = green;
        OCR1C = blue;
    }
}

void led_enable(int enabled) {
    if (enabled) {
        TIMSK |= (1 << OCIE0); // enable timer0 compare match interrupt
        OCR1A = 0xff;
        rgb_mode = RGB_R;
    } else {
        TIMSK &= ~(1 << OCIE0); // disable timer0 compare match interrupt
        OCR1A = 0x00;
        OCR1B = 0x00;
        OCR1C = 0x00;
        rgb_mode = RGB_NONE;
    }
}

void led_init() {
    // Set timer0 for adjust RGB value
    TCCR0 |= (1 << WGM01); // CTC mode
    TCCR0 |= (1 << CS02) | (1 << CS01); // prescaler 256
    // Calculate OCR0 for 4ms
    // 2ms = ORC0 * (1 / (clk / prescaler))
    // OCR0 = 2ms * (clk / prescaler) = 4ms * (16MHz / 256) = 125
    OCR0 = 125;

    // Set timer1 for make 3 PWM
    // Timer Mode : 8 bit fast PWM Mode (WGM1 = 0101)
    // Clear OC1A, OC1B, OC1C on Compare Match, set at Bottom (non-intverting mode, COM1A = 10, COM1B = 10, COM1C = 10)
    TCCR1A = (1 << COM1A1) | (1 << COM1B1) | (1 << COM1C1) | (1 << WGM10);
    TCCR1B = (1 << WGM12) | (1 << CS10);

    // Set initial duty ratio
    OCR1A = 0;
    OCR1B = 0;
    OCR1C = 0;

    // Set PB5, PB6, PB7 as PWM output
    DDRB |= (1 << PB5) | (1<< PB6) | (1 << PB7);
}