#include <avr/io.h>
#include <avr/iom128.h>
#include <stdint.h>
#include <avr/interrupt.h>

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

/**
 * @brief led_state_machine에서 변경된 rgb 값을 각각의 PWM의 Duty Rate로 바꿔주는 Method
*/
void led_set_color() {
    OCR1A = red;
    OCR1B = green;
    OCR1C = blue;
}

/**
 * @brief 현재 상태에 따라 rgb값을 바꿔주는 State Machine
*/
void led_state_machine() {
    switch (rgb_mode) {
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
    led_set_color();
}

void led_enable(int enabled) {
    if (enabled) {
        red = 0xff;
        green = 0x00;
        blue = 0x00;
        rgb_mode = RGB_R;
    } else {
        red = 0x00;
        green = 0x00;
        blue = 0x00;
        OCR1A = 0x00;
        OCR1B = 0x00;
        OCR1C = 0x00;
        rgb_mode = RGB_NONE;
    }
}

void led_init() {
    // Set PB5, PB6, PB7 as PWM output
    DDRB |= (1 << PB5) | (1<< PB6) | (1 << PB7);
}