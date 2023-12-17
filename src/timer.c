/* Timer와 Timeer Interrupt를 사용하기 위한 Library */
#include <avr/io.h>
#include <avr/iom128.h>
#include <avr/interrupt.h>
#include <stdint.h>

#include "timer.h"

/* Timer Interrupt에서 실행될 Method들이 있는 Module*/
#include "led.h"
#include "cds.h"

// 1ms마다 1씩 증가하는 counter
// overflow는 약 49.7일에 한번씩 일어난다.
volatile uint32_t timer0_counter;

// Timer0 Interrupt Service Routine
ISR(TIMER0_COMP_vect) {
    timer0_counter += 1;
    // For each 50ms
    if (timer0_counter % 50 == 0) {
        led_state_machine();
    }

    // For each 100ms
    if (timer0_counter % 100 == 0) {
        cds_sense();
        int led_current_state = led_onoff & ((!led_auto) | (led_auto & cds_is_sensed));
        if (led_current_state != led_is_enable) {
            led_is_enable = led_onoff & ((!led_auto) | (led_auto & cds_is_sensed));
            led_enable(led_is_enable);
        }
    }
}

/**
 * @brief Timer0 초기화 함수
*/
void timer0_init() {
    /**
     * Timer0이 1ms의 주기로 Timer Interrupt를 발생한다
     * Timer0의 Timer Interrupt에서는 timer0_counter의 값을 1 증가시키고,
     * timer0_counter의 값에 따라 추가적인 기능을 수행해준다
    */

    // Timer0 레지스터 설정
    // 1. TCCR0
    // Mode : CTC Mode (WGM0 = 10)
    // Compare Output 사용 안 함 (COM0 = 00)
    // Prescaler = 64 (CS0 = 100)
    TCCR0 |= (1 << WGM01) | (1 << CS02);

    // 2. OCR0
    // 주기 : 1ms
    // 1ms = OCR0 * (1 / (clk / prescaler))
    // OCR0 = 1ms * (clk / prescaler) = 1ms * (16MHz / 64) = 250
    OCR0 = 250;

    // 3. TIMSK
    // Timer 0번 Output Compare Match Interrup Enable (OCIE0 = 1)
    TIMSK |= (1 << OCIE0);
}

/**
 * @brief Timer1 초기화 함수
*/
void timer1_init() {
    /**
     * Timer1은 0 ~ 255의 범위의 Duty Ratio를 가지는 PWM을 생성한다.
     * OCR1A ~ OCR1C의 값에 따라 Duty Ratio가 정해지고
     * 이에 맞게 생성된 PWM이 PB5 ~ PB7에 출력된다
    */

    // Timer1 레지스터 설정
    // 1. TCCR1
    // Mode : 8 bit fast PWM (WGM1 = 0101)
    // Compare Output Mode : non-inverting mode(COM1A ~ COM1C = 10)
    // Prescaler : No prescaler (CS1 = 001)
    TCCR1A |= (1 << WGM10) | (1 << COM1A1) | (1 << COM1B1) | (1 << COM1C1);
    TCCR1B |= (1 << WGM12) | (1 << CS10);

    // 2. OCR1A ~ OCR1C
    // 초기화할 때에는 PWM이 생성되지 않게 0으로 초기화
    OCR1A = 0x00;
    OCR1B = 0x00;
    OCR1C = 0x00;
}

/**
 * @brief 사용하는 모든 Timer를 초기화하는 함수
*/
void timer_init() {
    timer0_init();
    timer1_init();
}