// i2c.c
//
// 19-10-2019
//


#include "i2c.h"


/*
//-----------------------------------------------------------------------------
void i2c_init(void)
    {
    GPIO_InitTypeDef GPIO_InitStruct;

    HAL_GPIO_WritePin(SW_I2C_PORT, SW_I2C_SDA_PIN , GPIO_PIN_SET);

    GPIO_InitStruct.Pin = SW_I2C_SCL_PIN | SW_I2C_SDA_PIN ;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(SW_I2C_PORT, &GPIO_InitStruct);
    }
*/


//-----------------------------------------------------------------------------
static void i2c_start(void)
    {
    SW_I2C_SDA_PULL;
    SW_I2C_DELAY;
    SW_I2C_SCL_PULL;
    SW_I2C_DELAY;

    SW_I2C_SDA_NULL;
    SW_I2C_DELAY;
    SW_I2C_SCL_NULL;
    SW_I2C_DELAY;
    }


//-----------------------------------------------------------------------------
void i2c_stop(void)
    {
    SW_I2C_SCL_NULL;
    SW_I2C_DELAY;
    SW_I2C_SDA_NULL;
    SW_I2C_DELAY;

    SW_I2C_SCL_PULL;
    SW_I2C_DELAY;
    SW_I2C_SDA_PULL;
    SW_I2C_DELAY;
    }


//-----------------------------------------------------------------------------
void i2c_send_byte(uint8_t data)
    {
    for(uint8_t mask=0x80; mask; mask>>=1)
        {
        if(data & mask) { SW_I2C_SDA_PULL; }
        else { SW_I2C_SDA_NULL; }

        SW_I2C_DELAY;
        SW_I2C_SCL_PULL;
        SW_I2C_DELAY;
        SW_I2C_SCL_NULL;
        }

    SW_I2C_SDA_PULL;
    SW_I2C_DELAY;
    SW_I2C_SCL_PULL;
    SW_I2C_DELAY;
    SW_I2C_SCL_NULL;
    }

/*
//-----------------------------------------------------------------------------
void i2c_send_data16(uint16_t data)
    {
    i2c_send_byte(data>>8);
    i2c_send_byte(data);
    }
*/

//-----------------------------------------------------------------------------
void i2c_start_addr(uint8_t addr)
    {
    i2c_start();
    i2c_send_byte(addr);
    }

/*
//-------------------------------------------------------------------------------------------------
void i2c_write(uint8_t addr, uint8_t *data, uint16_t size)
    {
    i2c_start();
    i2c_send_byte(addr);
    for(uint16_t i=0; i<size; i++) i2c_send_byte(*data++);
    i2c_stop();
    }
*/


//-----------------------------------------------------------------------------
uint8_t i2c_read_byte(uint8_t ack)
    {
    GPIO_InitTypeDef GPIO_InitStruct;

    int32_t data=0;

    SW_I2C_SDA_PULL;

    for(uint8_t i=0; i<8; i++)
        {
        data<<=1;
        SW_I2C_SCL_PULL;
        SW_I2C_DELAY;

        GPIO_InitStruct.Pin = SW_I2C_SDA_PIN ;
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(SW_I2C_PORT, &GPIO_InitStruct);

        if(SW_I2C_SDA_STAT) data |= 0x01;

        HAL_GPIO_WritePin(SW_I2C_PORT, SW_I2C_SDA_PIN , GPIO_PIN_SET);

        GPIO_InitStruct.Pin = SW_I2C_SDA_PIN ;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

        HAL_GPIO_Init(SW_I2C_PORT, &GPIO_InitStruct);

        SW_I2C_SCL_NULL;
        SW_I2C_DELAY;
        }

    if(ack==SW_I2C_ACK) { SW_I2C_SDA_NULL; }
    else { SW_I2C_SDA_PULL; }

    SW_I2C_DELAY;
    SW_I2C_SCL_PULL;
    SW_I2C_DELAY;
    SW_I2C_SCL_NULL;
    SW_I2C_DELAY;
    SW_I2C_SDA_PULL;

    return data;
    }


