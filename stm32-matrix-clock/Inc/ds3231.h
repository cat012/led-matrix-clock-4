// ds3231.h
//
// RTC DS3231
//
// 11-oct-2019
//


#ifndef DS3231_H_
#define DS3231_H_



#include <stdint.h>

#include "i2c.h"



//DS3231 I2C address
#define DS3231 0b11010000
#define WRITE  0b00000000
#define READ   0b00000001


#define SECONDS_REG  0x00
#define MINUTES_REG  0x01
#define HOURS_REG    0x02
#define DAY_REG      0x03
#define DATE_REG     0x04
#define MONTH_REG    0x05
#define YEAR_REG     0x06

#define CONTROL_REG  0x0e
#define STATUS_REG   0x0f


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

void rtc_set(uint8_t *newrtcdata);



#endif //DS3231_H_


