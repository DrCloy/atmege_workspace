#ifndef LED_H
#define LED_H

extern volatile int led_onoff;
extern volatile int led_auto;
extern volatile int led_is_enable;

void led_state_machine();
void led_enable(int enabled);
void led_init();

#endif