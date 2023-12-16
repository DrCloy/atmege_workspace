#include <avr/io.h>
#include <avr/iom128.h>

void cnd_init() {
    // ADC Mux Select
    // Voltage Reference = 5V (REFS = '00')
    // ADC Data Right Adjust (ADLAR = '0')
    // Mux : ADC0 Single Ended (Mux = '00000')
    ADMUX = 0x00;
    // ADC Control
    // ADC Enable (ADEN = '1')
    ADCSRA |= (1 << ADEN);
    
}