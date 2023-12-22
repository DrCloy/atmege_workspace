#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/iom128.h>
#include <stdint.h>
#include <util/delay.h>

#include "fnd.h"

#define FND_DIGIT        DDRC
#define FND_DIGIT_DATA   PORTC
#define FND_SELECT       DDRG
#define FND_SELECT_DATA  PORTG
#define FND_SEL_0        PG0
#define FND_SEL_1        PG1
#define FND_SEL_2        PG2
#define FND_SEL_3        PG3
#define FND_DP           PC7

/**
 * @brief FND 관련 초기화 함수
*/
void fnd_init() {
    FND_DIGIT = 0xff;
    FND_SELECT = ((1 << FND_SEL_0) | (1 << FND_SEL_1) | (1 << FND_SEL_2) | (1 << FND_SEL_3));
}

/**
 * @brief 입력된 값을 0 ~ 9, A ~ Z, a ~ z 중 하나로 판단하고, FND의 출력 형태로 바꿔주는 함수
 * 
 * @param value 문자 0 ~ 9, 'A' ~ 'Z', 'a' ~ 'z' 중 하나
 * @return FND 출력 형식에 맞도록 변형된 PORTC 레지스터에 입력될 값 
*/
uint8_t fnd_get_value(char value) {
    if (value >= '0' && value <= '9') {
        return fnd_value[value - '0'];
    } else if (value >= 'A' && value <= 'Z') {
        return fnd_value[value - 'A' + 10];
    } else if (value >= 'a' && value <= 'z') {
        return fnd_value[value - 'a' + 10];
    } else {
        return 0x00;
    }
}

/**
 * @brief FND에 값을 출력하는 함수
 * 
 * @param value PORTC의 값으로 사용될 값이 들어있는 Array
 * @param dp_index FND의 Digit Point를 찍을 위치, 없으면 -1
*/
void fnd_write(uint8_t *value, int dp_index) {
    for (int i = 0; i < 4; i++) {
        FND_DIGIT_DATA = value[i] | ((i == dp_index) ? (1 << FND_DP) : 0);
        FND_SELECT_DATA = 1 << (3 - i);
        _delay_ms(1);
    }
}

/**
 * @brief 외부에서 FND에게 값을 출력하도록 요청하는 함수
 * 
 * @param value FND에 출력될 문자가 담긴 Array
 * @param dp_index FND의 Digit Point를 찍을 위치, 없으면 -1
*/
void fnd_print(char *value, int dp_index) {
    uint8_t fnd_value[4];
    for (int i = 0; i < 4; i++) {
        fnd_value[i] = fnd_get_value(value[i]);
    }

    fnd_write(fnd_value, dp_index);
}