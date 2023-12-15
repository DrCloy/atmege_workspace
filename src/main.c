#include <avr/io.h>
#include <avr/iom128.h>
#include <avr/interrupt.h>

#include "fnd.h"
#include "ds3231.h"
#include "switch.h"
#include "led.h"

static const struct menu_t {
	int index;
	void (*func)(void);
} menu[] = {
	{0, fnd_print_time},
};

void init() {
	ds3231_init();
	fnd_init();
	switch_init();
	led_init();
	led_enable(1);
	sei();
}

void set_time() {
	rtc_t settime_time = {0, 0, 0};
	int setting = 0;

	while(1) {
		int sw = switch_read();

		if (sw == SWITCH_EVENT_BOTH) {
			setting += 1;
			if (setting == 2) {
				break;
			}
		} else if (sw == SWITCH_EVENT_UP) {
			if (setting == SETTING_HOUR) {
				settime_time.hour += 1;
			}
		}
	}
}

int main() {
	// const uint8_t menu_index_max = sizeof(menu) / sizeof(struct menu_t) - 1;
	const uint8_t menu_index_max = 10;
	int menu_index = 0;

	init();
	while (1) {
        fnd_print_function(menu_index);
        
        enum switch_event_t sw = switch_read();
        if (sw == SWITCH_EVENT_BOTH)
            menu[menu_index].func();
        if (sw == SWITCH_EVENT_DOWN)
            menu_index--;
        if (sw == SWITCH_EVENT_UP)
            menu_index++;
        
        if (menu_index < 0)
            menu_index = menu_index_max;
        if (menu_index > menu_index_max)
            menu_index = 0;
	}
}