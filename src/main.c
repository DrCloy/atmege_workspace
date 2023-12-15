#include <avr/io.h>
#include <avr/iom128.h>
#include <avr/interrupt.h>
#include "ds3231.h"
#include "fnd.h"
#include "led.h"
#include "switch.h"

static const struct menu_t {
	int index;
	void (*func)(void);
} menu[] = {
	{0, fnd_print_time},
};

rtc_t initTime = {0, 0, 0};

void init() {
	ds3231_init();
	ds3231_setTime(initTime);
	fnd_init();
	led_init();
	led_enable(1);
	sei();
}

int main() {
	rtc_t currentTime;

    const uint8_t menu_index_max = sizeof(menu) / sizeof(struct menu_t) - 1;
	int menu_index = 0;

	init();
	while (1) {
        fnd_print_function(menu_index);
        
        enum switch_event_t sw = switch_wait_until_input();
        if (sw == SWITCH_EVENT_BOTH){
			while (!switch_read()) {
				menu[menu_index].func();
			}
		}
        if (sw == SWITCH_EVENT_LEFT)
            menu_index--;
        if (sw == SWITCH_EVENT_RIGHT)
            menu_index++;
        
        if (menu_index < 0)
            menu_index = menu_index_max;
        if (menu_index > menu_index_max)
            menu_index = 0;
	}
}