#ifndef FND_H
#define FND_H

#define SETTING_HOUR 0
#define SETTING_MIN 1

void fnd_init();
void fnd_print_function(int index);
void fnd_print_time();
void fnd_print_setting_time(rtc_t setting_time, int hour_min);

#endif