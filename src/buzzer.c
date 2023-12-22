#include <avr/io.h>
#include <avr/iom128.h>
#include <stdint.h>

#include "timer.h"

#define BUZZER_OUTPUT PB4

volatile int buzzer_is_on = 0;
volatile int buzzer_default = 0;

const volatile char buzzer_scale[] = {'C', 'D', 'E', 'F', 'G', 'A', 'B'};
const volatile float buzzer_tone[] = {32.7032, 36.7081, 41.2034, 43.6535, 48.9994, 55.0000, 61.7354}; // C0 ~ B0


/**
 * @brief 부저 초기화 함수
*/
void buzzer_init() {
    // PB4 Output Mode
    DDRB |= (1 << BUZZER_OUTPUT);
}

/**
 * @brief 음계의 기본 주파수와 옥타브 수로 실제 음의 주파수를 구하는 함수
 * 
 * @param scale 음계의 기본 주파수
 * @param octave 옥타브 수, 기본 음일 때 0
*/
float buzzer_get_frequency(float scale, int octave) {
    float buzzer_frequency = scale * (1 << octave);

    return buzzer_frequency;
}

/**
 * @brief 음계의 기본 주파수와 옥타브 수로 실제 음을 내기 위한 Timer Register를 조절하는 함수
 * 
 * @param scale 음계의 기본 주파수
 * @param octave 옥타브 수, 기본 음일 때 0
*/
void buzzer_set_register(float scale, int octave) {
    // 음의 주파수 구하기
    // 음의 원래 주기 내에 toggle이 2번 일어나야 하므로 주기의 절반을 구해준다.
    float buzzer_frequency = buzzer_get_frequency(scale, octave) * 2;

    // 음의 주파수로 OCR0, Prescaler 구하기
    // 주기 = 1 / buzzer_frequency이다
    // 주기 = OCR0 * (1 / (clk / prescaler))
    // OCR0 = 주기 * (clk / prescaler) = (1 / buzzer_frequency) * (16000000 / prescaler)
    // Timer0이 가질 수 있는 prescaler : 1, 8, 32, 64, 128, 256, 1024
    // prescaler가 큰 값부터 차례대로 넣어보면서 OCR0이 1 ~ 255 사이에 오는 prescaler와 이 때의 OCR0을 선택한다

    // 1. prescaler 초기화 (TCCR0 CS0 = 000)
    TCCR0 &= ~((1 << CS00) | (1 << CS01) | (1 << CS02));
    OCR0 = 0;

    // 2. prescaler와 OCR0 구하기
    if ((buzzer_frequency * 1) > 62500) {
        TCCR0 |= (1 << CS00);
        OCR0 = (int)(16000000 / buzzer_frequency) * buzzer_is_on;
    } else if ((buzzer_frequency * 8) > 62500) {
        TCCR0 |= (1 << CS01);
        OCR0 = (int)(2000000 / buzzer_frequency) * buzzer_is_on;
    } else if ((buzzer_frequency * 32) > 62500) {
        TCCR0 |= (1 << CS01) | (1 << CS00);
        OCR0 = (int)(500000 / buzzer_frequency) * buzzer_is_on;
    } else if ((buzzer_frequency * 64) > 62500) {
        TCCR0 |= (1 << CS02);
        OCR0 = (int)(250000 / buzzer_frequency) * buzzer_is_on;
    } else if ((buzzer_frequency * 128) > 62500) {
        TCCR0 |= (1 << CS02) | (1 << CS00);
        OCR0 = (int)(125000 / buzzer_frequency) * buzzer_is_on;
    } else if ((buzzer_frequency * 256) > 62500) {
        TCCR0 |= (1 << CS02) | (1 << CS01);
        OCR0 = (int)(62500 / buzzer_frequency) * buzzer_is_on;
    } else if ((buzzer_frequency * 1024) > 62500) {
        TCCR0 |= (1 << CS02) | (1 << CS01) | (1 << CS00);
        OCR0 = (int)(15625 / buzzer_frequency) * buzzer_is_on;
    }
}

void buzzer_make_sound(int buzzer_value) {
    float scale = buzzer_tone[buzzer_value % 7];
    int octave = buzzer_value / 7;

    buzzer_set_register(scale, octave);
}

void buzzer_onoff(int onoff) {
    DDRB &= ~(1 << BUZZER_OUTPUT);
    DDRB |= (onoff << BUZZER_OUTPUT);
}