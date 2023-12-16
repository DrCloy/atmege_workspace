/* Timer와 Timeer Interrupt를 사용하기 위한 Library */
#include <avr/io.h>
#include <avr/iom128.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "timer.h"

/* Timer Interrupt에서 실행될 Method들이 있는 Module*/
#include "led.h"
#include "switch.h"

/**
 * @brief Timer0 초기화 함수
*/
void timer0_init() {
    /**
     * 
    */
}

/**
 * @brief 사용하는 모든 Timer를 초기화하는 함수
*/
void timer_init() {
    timer0_init();
}