// at24c32.c
//
// 17-10-2019
//


#include "at24c32.h"


//-----------------------------------------------------------------------------
void ee_write(uint16_t addr, uint8_t data)
    {
    i2c_start_addr(AT24C32_I2C_WRITE);  //device address | write
    i2c_send_byte(addr>>8);             //word address
    i2c_send_byte(addr);
    i2c_send_byte(data);                //write data word
    i2c_stop();

    AT24C32_DELAY_WRITE;
    }


//-----------------------------------------------------------------------------
uint8_t ee_read(uint16_t addr)
    {
    uint8_t data = 0;

    i2c_start_addr(AT24C32_I2C_WRITE);  //device address | write
    i2c_send_byte(addr>>8);             //word address
    i2c_send_byte(addr);
    i2c_start_addr(AT24C32_I2C_READ);   //device address | read
    data = i2c_read_byte(SW_I2C_NACK);
    i2c_stop();

    return data;
    }


//-----------------------------------------------------------------------------
void ee_clear(void)
    {
    for(uint16_t k=0; k<4096; k++) ee_write(k, 0xff);
    }



