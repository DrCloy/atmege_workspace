#include <avr/io.h>
#include <avr/iom128.h>
#include <avr/interrupt.h>
#include "ds3231.h"
#include "fnd_clock.h"
#include "led.h"

rtc_t testTime = {0, 5, 14};

int main() {
  ds3231_init();
  fnd_init();
  ds3231_setTime(testTime);
  led_init();
  led_enable(1);
  sei();
  
  rtc_t currentTime;
  while (1) {
    ds3231_getTime(&currentTime);
    fnd_print(currentTime.hour, currentTime.min);
  }
}