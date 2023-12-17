#ifndef CDS_H
#define CDS_H

#include <stdint.h>

// 현재 외부가 밝은 상태인지 어두운 상태인지를 나타냄
extern volatile int cds_is_sensed;
// Cds의 저항 임계값, 현재 Cds의 저항값이 이 값보다 크면 주변이 어둡다고 판단
extern volatile uint16_t cds_threshold;

void cds_init();
void cds_sense();

#endif