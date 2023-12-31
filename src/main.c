#include <avr/io.h>
#include <avr/iom128.h>
#include <avr/interrupt.h>

#include "cds.h"
#include "led.h"
#include "fnd.h"
#include "ds3231.h"
#include "switch.h"
#include "timer.h"
#include "buzzer.h"
#include "alarm.h"

// Menu 0 - 현재 시각 출력
void print_time() {
	rtc_t current_time;
	char fnd_value[4];

	while(!switch_read()) {
		// DS3231로부터 현재 시간 받아오기
		ds3231_getTime(&current_time);

		// 현재 시간 출력
		fnd_value[0] = '0' + (current_time.hour / 10);
		fnd_value[1] = '0' + (current_time.hour % 10);
		fnd_value[2] = '0' + (current_time.min / 10);
		fnd_value[3] = '0' + (current_time.min % 10);
		fnd_print(fnd_value, 1);
	}
}

// Menu 1 - 현재 시각 설정
void set_time() {
	rtc_t setting_time = {0, 0, 0};
	char fnd_value[4];
	int setting = 0;
	int sw;

	while(1) {
		sw = switch_read();

		// 현재 시각으로 설정할 시각 조절
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

		// 세팅 시간 출력
		fnd_value[0] = ('0' + (setting_time.hour / 10)) * (setting == SETTING_HOUR);
		fnd_value[1] = ('0' + (setting_time.hour % 10)) * (setting == SETTING_HOUR);
		fnd_value[2] = ('0' + (setting_time.min / 10)) * (setting == SETTING_MIN);
		fnd_value[3] = ('0' + (setting_time.min % 10)) * (setting == SETTING_MIN);
		fnd_print(fnd_value, 1);
	}
	// 현재 시각 반영
	ds3231_setTime(setting_time);
}

// Menu 2 - Cds 임계값 설정 
void set_cds_threshold() {
	int sw;
	char fnd_value[4];

	while(1) {
		sw = switch_read();

		if (sw == SWITCH_EVENT_BOTH) {
			break;
		} else if (sw == SWITCH_EVENT_UP) {
			cds_threshold += 1;
		} else if (sw == SWITCH_EVENT_DOWN) {
			cds_threshold -= 1;
			if (cds_threshold < 0) {
				cds_threshold = 0;
			}
		}

		// 임계값 출력
		fnd_value[0] = '0' + (cds_threshold / 1000);
		fnd_value[1] = '0' + ((cds_threshold / 100) % 10);
		fnd_value[2] = '0' + ((cds_threshold / 10) % 10);
		fnd_value[3] = '0' + (cds_threshold % 10);
		fnd_print(fnd_value, -1);
	}
}

// Menu 3 - LEN On/Off 설정
void set_led_onoff() {
	int sw;
	char fnd_value[4];

	while(1) {
		sw = switch_read();

		if (sw == SWITCH_EVENT_BOTH) {
			break;
		} else if ((sw == SWITCH_EVENT_UP) || (sw == SWITCH_EVENT_DOWN)) {
			led_onoff = !(led_onoff);
		}

		fnd_value[0] = 0;
		if (led_onoff) {
			fnd_value[1] = 0;
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
	char fnd_value[4];

	while(1) {
		sw = switch_read();

		if (sw == SWITCH_EVENT_BOTH) {
			break;
		} else if ((sw == SWITCH_EVENT_UP) || (sw == SWITCH_EVENT_DOWN)) {
			led_auto = !(led_auto);
		}

		fnd_value[0] = 0;
		if (led_auto) {
			fnd_value[1] = 0;
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

// Menu 5 : 부저의 테스트 및 부저 기본 음 설정
void set_buzzer() {
	int sw;
	int buzzer_value = -1;
	char fnd_value[4];

	buzzer_is_on = 1;
	buzzer_onoff(1);

	while(1) {
		sw = switch_read();

		if (sw == SWITCH_EVENT_BOTH) {
			break;
		} else if (sw == SWITCH_EVENT_UP) {
			buzzer_value++;		
			buzzer_make_sound(buzzer_value);
		} else if (sw == SWITCH_EVENT_DOWN) {
			buzzer_value--;		
			buzzer_make_sound(buzzer_value);
		}

		if (buzzer_value < -1) {
			buzzer_value = -1;
		}

		if (buzzer_value < 0) {
			fnd_value[0] = 'N';
			fnd_value[1] = 'O';
			fnd_value[2] = 'N';
			fnd_value[3] = 'E';
		} else {
			fnd_value[0] = 0;
			fnd_value[1] = 0;
			fnd_value[2] = buzzer_scale[buzzer_value % 7];
			fnd_value[3] = '0' + (buzzer_value / 7);
		}

		fnd_print(fnd_value, -1);
	}
	buzzer_is_on = 0;
	buzzer_default = buzzer_value;
	buzzer_onoff(0);
}

// Menu 6 : 알람이 울릴 시각 설정
void set_alarm() {
	rtc_t alarm_time = {0, 0, 0};
	char fnd_value[4];
	int setting = 0;
	int sw;

	while(1) {
		sw = switch_read();

		// 알람 시각 조절
		if (sw == SWITCH_EVENT_BOTH) {
			setting += 1;
			if (setting == 2) {
				break;
			}
		} else if (sw == SWITCH_EVENT_UP) {
			if (setting == SETTING_HOUR) {
				alarm_time.hour += 1;
				if (alarm_time.hour > 23) {
					alarm_time.hour = 0;
				}
			} else if (setting == SETTING_MIN) {
				alarm_time.min += 1;
				if (alarm_time.min > 59) {
					alarm_time.min = 0;
				}
			}
		} else if (sw == SWITCH_EVENT_DOWN) {
			if (setting == SETTING_HOUR) {
				alarm_time.hour -= 1;
				if (alarm_time.hour < 0) {
					alarm_time.hour = 23;
				}
			} else if (setting == SETTING_MIN) {
				alarm_time.min -= 1;
				if (alarm_time.min < 0) {
					alarm_time.min = 59;
				}
			}
		}

		// 알람 시간 출력
		fnd_value[0] = ('0' + (alarm_time.hour / 10)) * (setting == SETTING_HOUR);
		fnd_value[1] = ('0' + (alarm_time.hour % 10)) * (setting == SETTING_HOUR);
		fnd_value[2] = ('0' + (alarm_time.min / 10)) * (setting == SETTING_MIN);
		fnd_value[3] = ('0' + (alarm_time.min % 10)) * (setting == SETTING_MIN);
		fnd_print(fnd_value, 1);
	}
	// 알람 시각 반영
	alarm_set_alarm(alarm_time);
	alarm_is_added = 1;
}

// Menu 7 : 알람 기능 On/Off
void set_alarm_onoff() {
	int sw;
	int setting = 0;
	char fnd_value[4];
	fnd_value[0] = 0;

	while(1) {
		sw = switch_read();
		if (sw == SWITCH_EVENT_BOTH) {
			break;
		} else if (sw == SWITCH_EVENT_UP || sw == SWITCH_EVENT_DOWN) {
			setting = !(setting);
		}

		if (setting) {
			fnd_value[1] = 0;
			fnd_value[2] = 'O';
			fnd_value[3] = 'N';
		} else {
			fnd_value[1] = 'O';
			fnd_value[2] = 'F';
			fnd_value[3] = 'F';
		}

		fnd_print(fnd_value, -1);
	}

	alarm_is_on = setting;
}

// Menu 8 : 알람 끄기
void set_alarm_off() {
	int sw;
	char fnd_value[4];

	fnd_value[0] = 0;
	fnd_value[1] = 0;
	fnd_value[2] = 'O';
	fnd_value[3] = 'N';

	while(alarm_is_start) {
		sw = switch_read();

		if (sw == SWITCH_EVENT_BOTH) {
			alarm_is_start = 0;
			alarm_is_on = 0;
			buzzer_is_on = 0;
			buzzer_onoff(0);
		}

		fnd_print(fnd_value, -1);
	}

	fnd_value[0] = 0;
	fnd_value[1] = 'O';
	fnd_value[2] = 'F';
	fnd_value[3] = 'F';
	while(1) {
		sw = switch_read();

		if (sw == SWITCH_EVENT_BOTH) {
			break;
		}

		fnd_print(fnd_value, -1);
	}
}

// 기본 화면에서 메뉴 번호 출력
void print_menu(int menu_index) {
	char fnd_value[4];
	fnd_value[0] = 'F';
	fnd_value[1] = 'N';
	fnd_value[2] = '0' + (menu_index / 10);
	fnd_value[3] = '0' + (menu_index % 10);

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
	{5, set_buzzer},
	{6, set_alarm},
	{7, set_alarm_onoff},
	{8, set_alarm_off},
};

// 전체 초기화
void init() {
	timer_init();
	ds3231_init();
	fnd_init();
	switch_init();
	led_init();
	// led_enable(1);
	cds_init();
	buzzer_init();
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
        if (sw == SWITCH_EVENT_DOWN){
            menu_index--;
			if (menu_index < 0)
				menu_index = menu_index_max;
		}
        if (sw == SWITCH_EVENT_UP){
            menu_index++;
			if (menu_index > menu_index_max)
				menu_index = 0;
		}
        
	}
}