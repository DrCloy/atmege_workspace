#ifndef CDS_H
#define CDS_H

#include <stdint.h>

extern volatile int cds_is_sensed;
extern volatile uint16_t cds_threshold;

void cds_init();
void cds_sense();

#endif