#ifndef FND_H
#define FND_H

#include <stdint.h>
#include "ds3231.h"

void fnd_init();
void fnd_print_function(int index);
void fnd_print_time();

#endif