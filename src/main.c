#include <avr/io.h>
#include <avr/iom128.h>
#include <avr/interrupt.h>
#include "ds3231.h"
#include "fnd_clock.h"
#include "led.h"

static const struct menu_t {
  int index;
  void (*func)(void);
} menu[] = {
  {0, },
};

rtc_t initTime = {0, 0, 0};

void init() {
  ds3231_init();
  fnd_init();
  ds3231_setTime(initTime);
  led_init();
  led_enable(1);
  sei();
}

int main() {
  rtc_t currentTime;
  while (1) {
    ds3231_getTime(&currentTime);
    fnd_print_time(currentTime.hour, currentTime.min);
  }
}