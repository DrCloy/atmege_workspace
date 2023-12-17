#ifndef BUZZER_H
#define BUZZER_H

extern volatile int buzzer_is_on;
extern const char buzzer_scale[];

void buzzer_init();
void buzzer_make_sound(int buzzer_value);

#endif