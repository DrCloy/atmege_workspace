#include <avr/io.h>
#include <avr/iom128.h>
#include <stdint.h>

#include "timer.h"

#define BUZZER_OUTPUT PB4

void buzzer_init() {
    // PB4 Output Mode
    DDRB |= (1 << BUZZER_OUTPUT);
}

float buzzer_get_frequency(float scale, int octave) {
    float buzzer_frequency = scale * (1 << (octave + 1));

    return buzzer_frequency;
}

int buzzer_set_register(float scale, int octave) {
    float buzzer_frequency = buzzer_get_frequency(scale, octave);
}