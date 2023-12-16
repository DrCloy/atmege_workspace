#ifndef CDS_H
#define CDS_H

#include <stdint.h>
#include <stdbool.h>

void cds_init();
bool cds_state(uint16_t threshold);

#endif