// ds3231.c
//
// 11-oct-2019
//


#include "ds3231.h"


//-----------------------------------------------------------------------------
void rtc_conv_data(uint8_t *d, uint8_t m)  //d-data //m-mask
    {
    *d=(((((*d & m)>>4)<<1)+(((*d & m)>>4)<<3))+(*d & 0x0F));
    }


//-----------------------------------------------------------------------------
void rtc_read(uint8_t *data)
    {
    i2c_start();
    i2c_send_byte(DS3231 | WRITE);

    i2c_send_byte(0x00);

    i2c_start();

    i2c_send_byte(DS3231 | READ);

    for(uint8_t i=0; i<(RTC_DATA_SIZE-1); i++) data[i] = i2c_read_byte(I2C_ACK);

    data[RTC_DATA_SIZE-1] = i2c_read_byte(I2C_NACK);

    i2c_stop();

    rtc_conv_data(&data[SECONDS_REG], 0x70);
    rtc_conv_data(&data[MINUTES_REG], 0x70);
    rtc_conv_data(&data[HOURS_REG], 0x30);
    rtc_conv_data(&data[DATE_REG], 0x30);
    rtc_conv_data(&data[MONTH_REG], 0x10);
    rtc_conv_data(&data[YEAR_REG], 0xF0);
    }


//-----------------------------------------------------------------------------
void ds3231_write_reg(uint8_t addr, uint8_t val)
    {
    i2c_start();
    i2c_send_byte(DS3231 | WRITE);

    i2c_send_byte(addr);
    i2c_send_byte(val);

    i2c_stop();
    }


//-----------------------------------------------------------------------------
uint8_t ds3231_read_reg(uint8_t addr)
    {
    i2c_start();
    i2c_send_byte(DS3231 | WRITE);

    i2c_send_byte(addr);

    i2c_start();
    i2c_send_byte(DS3231 | READ);

    uint8_t data = i2c_read_byte(I2C_NACK);

    i2c_stop();

    return data;
    }


//-----------------------------------------------------------------------------
void ds3231_init(void)
    {
    ds3231_write_reg(CONTROL_REG, 0b00011100);
    ds3231_write_reg(STATUS_REG,  0b00000000);

    uint8_t temp = ds3231_read_reg(HOURS_REG);
    ds3231_write_reg(HOURS_REG, temp & 0b10111111); //set 24-hour mode
    }


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


//-----------------------------------------------------------------------------
void rtc_set(uint8_t *newrtcdata)
    {
    rtc_set_sec(newrtcdata[SECONDS_REG]);
    rtc_set_min(newrtcdata[MINUTES_REG]);
    rtc_set_hrs(newrtcdata[HOURS_REG]);
    rtc_set_day(newrtcdata[DAY_REG]);
    rtc_set_dat(newrtcdata[DATE_REG]);
    rtc_set_mon(newrtcdata[MONTH_REG]);
    rtc_set_year(newrtcdata[YEAR_REG]);
    }

