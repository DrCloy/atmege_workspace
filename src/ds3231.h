#ifndef DS3231_h
#define DS3231_h

#include <stdio.h>
#include <avr/io.h>
#include <avr/iom128.h>
#include "i2c.h"


#define  DS3231_ADDRESS      0x68
#define  DS3231_ADDRESS_W    (DS3231_ADDRESS << 1)
#define  DS3231_ADDRESS_R    ((DS3231_ADDRESS << 1) | 0x01)
#define  DS3231_REG_SEC		 0x00
#define  DS3231_REG_MIN		 0x01
#define  DS3231_REG_HOUR     0x02
#define  DS3231_REG_DOW		 0x03
#define  DS3231_REG_DATE     0x04
#define  DS3231_REG_MON		 0x05
#define  DS3231_REG_YEAR     0x06
#define  DS3231_REG_CON		 0x0E
#define  DS3231_REG_STATUS	 0x0F
#define  DS3231_REG_AGING	 0x10
#define  DS3231_REG_TEMPM	 0x11
#define  DS3231_REG_TEMPL	 0x12

typedef struct
{
	uint8_t sec;
	uint8_t min;
	uint8_t hour;
} rtc_t;



// Convert normal decimal numbers to binary coded decimal
static inline uint8_t decimalToBcd(uint8_t val){
    return( (val/10*16) + (val%10) );
}

// Convert binary coded decimal to normal decimal numbers
static inline uint8_t bcdToDec(uint8_t val){
    return( (val/16*10) + (val%16) );
}

static inline void ds3231_setTime(rtc_t realTime){
    i2c_start();
    i2c_write(DS3231_ADDRESS_W);
    i2c_write(0x00);    //move pointer location 0 which is seconds
    i2c_write(decimalToBcd(0)); // second will always be set 0 because second cannot be printed in fnd
    i2c_write(decimalToBcd(realTime.min));
    i2c_write(decimalToBcd(realTime.hour));  // when bit 6 is low, 24 hour mode
    i2c_stop();  
}

static inline void ds3231_getTime(rtc_t* realTime){
    
    i2c_start();
    i2c_write(DS3231_ADDRESS_W);
    i2c_write(0x00);            // set DS3231 register pointer to 00h
    i2c_stop();
    
    i2c_start();
    i2c_write(DS3231_ADDRESS_R);
    
    realTime->sec        = bcdToDec(i2c_read(1) & 0x7f);
    realTime->min        = bcdToDec(i2c_read(1));
    realTime->hour       = bcdToDec(i2c_read(1) & 0x3f);   
}

static inline void ds3231_init() {
    rtc_t init_time = {0, 0, 0};

    i2c_init();
    ds3231_setTime(init_time);
}

#endif /* DS3231_h */