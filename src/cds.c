#include <avr/io.h>
#include <avr/iom128.h>
#include <stdint.h>

#include "cds.h"

volatile int cds_is_sensed;
volatile uint16_t cds_threshold;

void cds_init() {
    /** ADC Mux Select */
    // Voltage Reference = 5V (REFS = '00')
    // ADC Data Right Adjust (ADLAR = '0')
    // Mux : ADC0 Single Ended (Mux = '00000')
    ADMUX = 0x00;
    /** ADC Control */ 
    // ADC Enable (ADEN = '1')
    ADCSRA |= (1 << ADEN);
    // Mode = Single conversion (ADFR = '0')
    // Prescaler = 32 (ADPS = '101')
    ADCSRA |= (1 << ADPS0) | (1 << ADPS2);

    cds_threshold = 5;
}

uint16_t cds_read() {
    uint8_t adc_low, adc_high;
    uint16_t value;
    // ADC Start (ADSC = '1')
    ADCSRA |= (1 << ADSC);
    // Check ADC Conversion Complete (Check ADIF = '1')
    // Wait until ADC Conversion Complete
    while ((ADCSRA & (1 << ADIF)) != (1 << ADIF));

    // Read Converted Value
    adc_low = ADCL;
    adc_high = ADCH;
    value = (adc_high << 8) | adc_low; // 10bit ADC value

    return value;
}

float cds_convert(uint16_t adc_value) {
    // Get Voltage of CDS
    // ADC Voltage (= 200K Register Voltage) = ADC Value / 1024 * 5
    // CDS Voltage = 5V - ADC Voltage = 5 - (ADC Value / 1024 * 5)
    float cds_voltage = 5 - ((float)(adc_value) / 1024 * 5);

    // Get Register of CDS
    // Voltage of CDS : Voltage of 200K Register(K Ohm) = Register of CDS : 200K
    // CDS Voltage : 5 - CDS Voltage = CDS Register : 200
    // CDS Register = (200 * CDS Voltage) / (5 - CDS Voltage)
    float cds_register = (200 * cds_voltage) / (5 - cds_voltage);

    return cds_register;
}

void cds_sense() {
    float cds_register = cds_convert(cds_read());

    cds_is_sensed = (cds_register > cds_threshold) ? 1 : 0;
}