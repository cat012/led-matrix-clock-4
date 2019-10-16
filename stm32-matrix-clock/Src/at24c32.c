// at24c32.c
//
// 23-05-2019
//


#include "at24c32.h"


//-----------------------------------------------------------------------------
void ee_write(uint16_t addr, uint8_t data)
    {
    i2c_start();
    i2c_send_byte(EE_W);     //device address | write
    i2c_send_byte(addr>>8);  //word address
    i2c_send_byte(addr);
    i2c_send_byte(data);     //write data word
    i2c_stop();

    EE_DELAY_WRITE;
    }


//-----------------------------------------------------------------------------
uint8_t ee_read(uint16_t addr)
    {
    uint8_t data = 0;

    i2c_start();
    i2c_send_byte(EE_W);     //device address | write
    i2c_send_byte(addr>>8);  //word address
    i2c_send_byte(addr);
    i2c_start();
    i2c_send_byte(EE_R);     //device address | read
    data = i2c_read_byte(I2C_NACK);
    i2c_stop();

    return data;
    }


//-----------------------------------------------------------------------------
void ee_clear(void)
    {
    for(uint16_t k=0; k<4096; k++) ee_write(k, 0xff);
    }



