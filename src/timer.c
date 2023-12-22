/* Timer와 Timeer Interrupt를 사용하기 위한 Library */
#include <avr/io.h>
#include <avr/iom128.h>
#include <avr/interrupt.h>
#include <stdint.h>

#include "timer.h"

/* Timer Interrupt에서 실행될 Method들이 있는 Module*/
#include "led.h"
#include "cds.h"
#include "buzzer.h"
#include "alarm.h"

// 1ms마다 1씩 증가하는 counter
// overflow는 약 49.7일에 한번씩 일어난다.
volatile uint32_t timer2_counter;

// Timer2 Interrupt Service Routine
ISR(TIMER2_COMP_vect) {
    timer2_counter += 1;
    // For each 50ms
    if (timer2_counter % 50 == 0) {
        led_state_machine();
    }

    // For each 100ms
    if (timer2_counter % 100 == 0 && led_auto) {
        cds_sense();
        int led_current_state = led_onoff & ((!led_auto) | (led_auto & cds_is_sensed));
        if (led_current_state != led_is_enable) {
            led_is_enable = led_onoff & ((!led_auto) | (led_auto & cds_is_sensed));
            led_enable(led_is_enable);
        }
    }

    // For each 1000ms = 1s
    if (timer2_counter % 1000 == 0) {
        if (alarm_is_added) {
            if (alarm_is_on && !(alarm_is_start) && alarm_check_alarm()) {
                alarm_is_start = 1;
                alarm_start_time = timer2_counter;
                buzzer_is_on = 1;
                buzzer_make_sound(buzzer_default);
                buzzer_onoff(1);
            } 
            // else if (alarm_is_start && ((timer2_counter - alarm_is_start) > ALARM_BUZZER_TIME)) {
            //     alarm_is_start = 0;
            //     buzzer_is_on = 0;
            //     buzzer_make_sound(0);
            // }

        }
    }
}

/**
 * @brief Timer0 초기화 함수
*/
void timer0_init() {
    /**
     * Timer0은 특정 주파수에 맞게 부저를 울리기 위해, Square Wave를 생성하는 timer이다
     * buzzer.c에서 구해진 음의 주파수에 맞게 prescaler와 OCR0이 정해지면
     * PB4에 Square Wave가 출력된다
    */

    // Timer0 레지스터 설정
    // 1. TCCR0
    // Mode : CTC Mode (WGM0 = 10)
    // Compare Output Mode : Toggle (COM0 = 01)
    // Prescaler는 buzzer.c에서 조절
    TCCR0 |= (1 << WGM01) | (1 << COM00); 
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
 * @brief Timer2 초기화 함수
*/
void timer2_init() {
    /**
     * Timer2이 1ms의 주기로 Timer Interrupt를 발생한다
     * Timer2의 Timer Interrupt에서는 timer0_counter의 값을 1 증가시키고,
     * timer2_counter의 값에 따라 추가적인 기능을 수행해준다
    */

    // Timer2 레지스터 설정
    // 1. TCCR2
    // Mode : CTC Mode (WGM2 = 10)
    // Compare Output 사용 안 함 (COM2 = 00)
    // Prescaler = 64 (CS2 = 011)
    TCCR2 |= (1 << WGM21) | (1 << CS21) | (1 << CS20);

    // 2. OCR2
    // 주기 : 1ms
    // 1ms = OCR2 * (1 / (clk / prescaler))
    // OCR2 = 1ms * (clk / prescaler) = 1ms * (16MHz / 64) = 250
    OCR2 = 250;

    // 3. TIMSK
    // Timer 2번 Output Compare Match Interrup Enable (OCIE2 = 1)
    TIMSK |= (1 << OCIE2);
}

/**
 * @brief 사용하는 모든 Timer를 초기화하는 함수
*/
void timer_init() {
    timer0_init();
    timer1_init();
    timer2_init();
}