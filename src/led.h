#ifndef LED_H
#define LED_H

// LED를 켜고 끄는 상태를 나타냄
extern volatile int led_onoff;
// LED가 Cds에 따라 자동적으로 켜지고 꺼지는 기능의 On/Off 유무
extern volatile int led_auto;
// 현재 LED가 켜진 상태인지 거진 상태인지 기록
extern volatile int led_is_enable;

void led_state_machine();
void led_enable(int enabled);
void led_init();

#endif