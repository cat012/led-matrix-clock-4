// ds3231.c
//
// 19-oct-2019
//


#include "ds3231.h"


const uint8_t drmask[RTC_DATA_SIZE]=
  {
  0b01110000,  // 0 seconds
  0b01110000,  // 1 minutes
  0b00110000,  // 2 hours
  0b00000000,  // 3 n/a
  0b00110000,  // 4 date
  0b00010000,  // 5 month
  0b11110000   // 6 year
  };


//-----------------------------------------------------------------------------
static void rtc_conv_data(uint8_t *d, const uint8_t m)  //d-data //m-mask
    {
    *d=(((((*d & m)>>4)<<1)+(((*d & m)>>4)<<3))+(*d & 0b00001111));
    }


//-----------------------------------------------------------------------------
void rtc_read(uint8_t *data)
    {
    i2c_start_addr(DS3231_I2C_WRITE);

    i2c_send_byte(0x00);  //address

    i2c_start_addr(DS3231_I2C_READ);

    for(uint8_t i=0; i<(RTC_DATA_SIZE-1); i++) data[i] = i2c_read_byte(SW_I2C_ACK);

    data[RTC_DATA_SIZE-1] = i2c_read_byte(SW_I2C_NACK);

    i2c_stop();

    for(uint8_t k=0; k<RTC_DATA_SIZE; k++) rtc_conv_data(&data[k], drmask[k]);
    }


//-----------------------------------------------------------------------------
void ds3231_write_reg(uint8_t addr, uint8_t val)
    {
    i2c_start_addr(DS3231_I2C_WRITE);

    i2c_send_byte(addr);
    i2c_send_byte(val);

    i2c_stop();
    }


//-----------------------------------------------------------------------------
uint8_t ds3231_read_reg(uint8_t addr)
    {
    i2c_start_addr(DS3231_I2C_WRITE);

    i2c_send_byte(addr);

    i2c_start_addr(DS3231_I2C_READ);

    uint8_t data = i2c_read_byte(SW_I2C_NACK);

    i2c_stop();

    return data;
    }


//-----------------------------------------------------------------------------
void ds3231_init(void)
    {
    ds3231_write_reg(DS3231_MAP_CONTROL, 0b00011100);
    ds3231_write_reg(DS3231_MAP_STATUS,  0b00000000);

    uint8_t temp = ds3231_read_reg(DS3231_MAP_HOURS);
    ds3231_write_reg(DS3231_MAP_HOURS, temp & 0b10111111); //set 24-hour mode
    }


/*
//-----------------------------------------------------------------------------
int8_t ds3231_temperature(void)
    {
    int8_t tempmsb = ds3231_read_reg(DS3231_MAP_MSBTEMP);

    return tempmsb;
    }


//-----------------------------------------------------------------------------
int16_t ds3231_temperature2(void)
    {
    int16_t tempmsb=ds3231_read_reg(DS3231_MAP_MSBTEMP);
    uint8_t templsb=ds3231_read_reg(DS3231_MAP_LSBTEMP);

    return (tempmsb*100)+((templsb>>6)*25);
    }
*/


//-----------------------------------------------------------------------------
void rtc_set_sec(uint8_t val)
    {
    ds3231_write_reg(0x00, (((val/10)<<4)+(val%10)) & 0b01111111);
    }


//-----------------------------------------------------------------------------
void rtc_set_min(uint8_t val)
    {
    ds3231_write_reg(0x01, (((val/10)<<4)+(val%10)) & 0b01111111);
    }


//-----------------------------------------------------------------------------
void rtc_set_hrs(uint8_t val)
    {
    ds3231_write_reg(0x02, (((val/10)<<4)+(val%10)) & 0b10111111);
    }


//-----------------------------------------------------------------------------
void rtc_set_day(uint8_t val)
    {
    ds3231_write_reg(0x03, (val & 0b00000111));
    }


//-----------------------------------------------------------------------------
void rtc_set_dat(uint8_t val)
    {
    ds3231_write_reg(0x04, (((val/10)<<4)+(val%10)) & 0b00111111);
    }


//-----------------------------------------------------------------------------
void rtc_set_mon(uint8_t val)
    {
    ds3231_write_reg(0x05, (((val/10)<<4)+(val%10)) & 0b00011111);
    }


//-----------------------------------------------------------------------------
void rtc_set_year(uint8_t val)
    {
    ds3231_write_reg(0x06, ((val/10)<<4)+(val%10));
    }


/*
//-----------------------------------------------------------------------------
void rtc_set(uint8_t *newrtcdata)
    {
    rtc_set_sec(newrtcdata[DS3231_MAP_SECONDS]);
    rtc_set_min(newrtcdata[DS3231_MAP_MINUTES]);
    rtc_set_hrs(newrtcdata[DS3231_MAP_HOURS]);
    rtc_set_day(newrtcdata[DS3231_MAP_DAY]);
    rtc_set_dat(newrtcdata[DS3231_MAP_DATE]);
    rtc_set_mon(newrtcdata[DS3231_MAP_MONTH]);
    rtc_set_year(newrtcdata[DS3231_MAP_YEAR]);
    }
*/


