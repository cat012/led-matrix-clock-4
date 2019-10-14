// i2c.c
//
// 26-05-2019
//


#include "i2c.h"


//-----------------------------------------------------------------------------
void i2c_init(void)
    {
    GPIO_InitTypeDef GPIO_InitStruct;

    HAL_GPIO_WritePin(I2C_PORT, I2C_SDA_PIN , GPIO_PIN_SET);

    GPIO_InitStruct.Pin = I2C_SCL_PIN | I2C_SDA_PIN ;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(I2C_PORT, &GPIO_InitStruct);
    }


//-----------------------------------------------------------------------------
void i2c_start(void)
    {
    SDA_PULL;
    I2C_DELAY;
    SCL_PULL;
    I2C_DELAY;

    SDA_NULL;
    I2C_DELAY;
    SCL_NULL;
    I2C_DELAY;
    }


//-----------------------------------------------------------------------------
void i2c_stop(void)
    {
    SCL_NULL;
    I2C_DELAY;
    SDA_NULL;
    I2C_DELAY;

    SCL_PULL;
    I2C_DELAY;
    SDA_PULL;
    I2C_DELAY;
    }


//-----------------------------------------------------------------------------
void i2c_send_byte(uint8_t data)
    {
    for(uint8_t mask=0x80; mask; mask>>=1)
        {
        if(data & mask) { SDA_PULL; }
        else { SDA_NULL; }

        I2C_DELAY;
        SCL_PULL;
        I2C_DELAY;
        SCL_NULL;
        }

    SDA_PULL;
    I2C_DELAY;
    SCL_PULL;
    I2C_DELAY;
    SCL_NULL;
    }


//-----------------------------------------------------------------------------
uint8_t i2c_read_byte(uint8_t ack)
    {
    GPIO_InitTypeDef GPIO_InitStruct;

    int32_t data=0;

    SDA_PULL;

    for(uint8_t i=0; i<8; i++)
        {
        data<<=1;
        SCL_PULL;
        I2C_DELAY;

        GPIO_InitStruct.Pin = I2C_SDA_PIN ;
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(I2C_PORT, &GPIO_InitStruct);

        if(SDA_STAT) data |= 0x01;

        HAL_GPIO_WritePin(I2C_PORT, I2C_SDA_PIN , GPIO_PIN_SET);

        GPIO_InitStruct.Pin = I2C_SDA_PIN ;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

        HAL_GPIO_Init(I2C_PORT, &GPIO_InitStruct);

        SCL_NULL;
        I2C_DELAY;
        }

    if(ack==I2C_ACK) { SDA_NULL; }
    else { SDA_PULL; }

    I2C_DELAY;
    SCL_PULL;
    I2C_DELAY;
    SCL_NULL;
    I2C_DELAY;
    SDA_PULL;

    return data;
    }


