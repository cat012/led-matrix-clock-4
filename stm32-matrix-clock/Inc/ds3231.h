// ds3231.h
//
// RTC DS3231
//
// 17-oct-2019
//


#ifndef DS3231_H_
#define DS3231_H_



#include <stdint.h>

#include "i2c.h"



//DS3231 I2C address
#define DS3231_I2C_WRITE    0b11010000
#define DS3231_I2C_READ     0b11010001


#define DS3231_MAP_SECONDS  0x00
#define DS3231_MAP_MINUTES  0x01
#define DS3231_MAP_HOURS    0x02
#define DS3231_MAP_DAY      0x03
#define DS3231_MAP_DATE     0x04
#define DS3231_MAP_MONTH    0x05
#define DS3231_MAP_YEAR     0x06

#define DS3231_MAP_CONTROL  0x0e
#define DS3231_MAP_STATUS   0x0f

#define DS3231_MAP_MSBTEMP   0x11
#define DS3231_MAP_LSBTEMP   0x12


#define RTC_DATA_SIZE  0x07


void ds3231_init(void);
void rtc_read(uint8_t *data);

uint8_t ds3231_read_reg(uint8_t addr);

void rtc_set_sec(uint8_t val);
void rtc_set_min(uint8_t val);
void rtc_set_hrs(uint8_t val);
void rtc_set_day(uint8_t val);
void rtc_set_dat(uint8_t val);
void rtc_set_mon(uint8_t val);
void rtc_set_year(uint8_t val);

//void rtc_set(uint8_t *newrtcdata);



#endif //DS3231_H_


