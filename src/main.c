#include <avr/io.h>
#include <avr/iom128.h>
#include <avr/interrupt.h>

#include "cds.h"
#include "led.h"
#include "fnd.h"
#include "ds3231.h"
#include "switch.h"
#include "timer.h"

// Menu 0 - 현재 시각 출력
void print_time() {
	rtc_t current_time;

	while(!switch_read()) {
		ds3231_getTime(&current_time);

		uint8_t fnd_value[4];
		fnd_value[0] = current_time.hour / 10;
		fnd_value[1] = current_time.hour % 10;
		fnd_value[2] = current_time.min / 10;
		fnd_value[3] = current_time.min % 10;

		fnd_print(fnd_value, 1);
	}
}

// Menu 1 - 현재 시각 설정
void set_time() {
	rtc_t setting_time = {0, 0, 0};
	uint8_t fnd_value[4];
	int setting = 0;
	int sw;

	while(1) {
		sw = switch_read();

		if (sw == SWITCH_EVENT_BOTH) {
			setting += 1;
			if (setting == 2) {
				break;
			}
		} else if (sw == SWITCH_EVENT_UP) {
			if (setting == SETTING_HOUR) {
				setting_time.hour += 1;
				if (setting_time.hour > 23) {
					setting_time.hour = 0;
				}
			} else if (setting == SETTING_MIN) {
				setting_time.min += 1;
				if (setting_time.min > 59) {
					setting_time.min = 0;
				}
			}
		} else if (sw == SWITCH_EVENT_DOWN) {
			if (setting == SETTING_HOUR) {
				setting_time.hour -= 1;
				if (setting_time.hour < 0) {
					setting_time.hour = 23;
				}
			} else if (setting == SETTING_MIN) {
				setting_time.min -= 1;
				if (setting_time.min < 0) {
					setting_time.min = 59;
				}
			}
		}

		fnd_value[0] = (setting_time.hour / 10) & (setting == SETTING_HOUR);
		fnd_value[1] = (setting_time.hour % 10) & (setting == SETTING_HOUR);
		fnd_value[2] = (setting_time.min / 10) & (setting == SETTING_MIN);
		fnd_value[3] = (setting_time.min % 10) & (setting == SETTING_MIN);

		fnd_print(fnd_value, 1);
	}
	ds3231_setTime(setting_time);
}

// Menu 2 - Cds 임계값 설정 
void set_cds_threshold() {
	int sw;
	uint8_t fnd_value[4];

	while(1) {
		sw = switch_read();

		if (sw == SWITCH_EVENT_BOTH) {
			break;
		} else if (sw == SWITCH_EVENT_UP) {
			cds_threshold += 1;
		} else if (sw == SWITCH_EVENT_DOWN) {
			cds_threshold -= 1;
		}

		fnd_value[0] = cds_threshold / 1000;
		fnd_value[1] = (cds_threshold / 100) % 10;
		fnd_value[2] = (cds_threshold / 10) % 10;
		fnd_value[3] = cds_threshold % 10;

		fnd_print(fnd_value, 1);
	}
}

// Menu 3 - LEN On/Off 설정
void set_led_onoff() {
	int sw;
	uint8_t fnd_value[4];

	while(1) {
		sw = switch_read();

		if (sw == SWITCH_EVENT_BOTH) {
			break;
		} else if ((sw == SWITCH_EVENT_UP) || (sw == SWITCH_EVENT_DOWN)) {
			led_onoff = !(led_onoff);
		}

		fnd_value[0] = 'z' + 1;
		if (led_onoff) {
			fnd_value[1] = 'z' + 1;
			fnd_value[2] = 'O';
			fnd_value[3] = 'N';
		} else {
			fnd_value[1] = 'O';
			fnd_value[2] = 'F';
			fnd_value[3] = 'F';
		}

		fnd_print(fnd_value, -1);
	}
}

// Menu 4 - 주변의 빛에 따라 LED 자동 On/Off 기능 설정 
void set_led_auto() {
	int sw;
	uint8_t fnd_value[4];

	while(1) {
		sw = switch_read();

		if (sw == SWITCH_EVENT_BOTH) {
			break;
		} else if ((sw == SWITCH_EVENT_UP) || (sw == SWITCH_EVENT_DOWN)) {
			led_auto = !(led_auto);
		}

		fnd_value[0] = 'z' + 1;
		if (led_auto) {
			fnd_value[1] = 'z' + 1;
			fnd_value[2] = 'O';
			fnd_value[3] = 'N';
		} else {
			fnd_value[1] = 'O';
			fnd_value[2] = 'F';
			fnd_value[3] = 'F';
		}

		fnd_print(fnd_value, -1);
	}
}

// 기본 화면에서 메뉴 번호 출력
void print_menu(int menu_index) {
	uint8_t fnd_value[4];
	fnd_value[0] = 'F';
	fnd_value[1] = 'N';
	fnd_value[2] = menu_index / 10;
	fnd_value[3] = menu_index % 10;

	fnd_print(fnd_value, 1);
}

// 기능들의 모음
static const struct menu_t {
	int index;
	void (*func)(void);
} menu[] = {
	{0, print_time},
	{1, set_time},
	{2, set_cds_threshold},
	{3, set_led_onoff},
	{4, set_led_auto},
};

// 전체 초기화
void init() {
	timer_init();
	ds3231_init();
	fnd_init();
	switch_init();
	led_init();
	led_enable(1);
	cds_init();
	sei();
}

// Main
int main() {
	const uint8_t menu_index_max = sizeof(menu) / sizeof(struct menu_t) - 1;
	int menu_index = 0;
	enum switch_event_t sw;

	init();
	
	// Loop
	while (1) {
        print_menu(menu_index);
        
        sw = switch_read();
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