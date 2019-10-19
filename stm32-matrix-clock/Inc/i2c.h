// i2c.h
//
// Software I2C
//
// 19-10-2019
//


#ifndef I2C_H_
#define I2C_H_


#include "stm32f0xx_hal.h"


#define SW_I2C_PORT     GPIOA
#define SW_I2C_SDA_PIN  GPIO_PIN_10
#define SW_I2C_SCL_PIN  GPIO_PIN_9


#define SW_I2C_SDA_PULL  (HAL_GPIO_WritePin(SW_I2C_PORT, SW_I2C_SDA_PIN , GPIO_PIN_SET))
#define SW_I2C_SDA_NULL  (HAL_GPIO_WritePin(SW_I2C_PORT, SW_I2C_SDA_PIN , GPIO_PIN_RESET))
#define SW_I2C_SDA_STAT  (HAL_GPIO_ReadPin(SW_I2C_PORT, SW_I2C_SDA_PIN ))

#define SW_I2C_SCL_PULL  (HAL_GPIO_WritePin(SW_I2C_PORT, SW_I2C_SCL_PIN , GPIO_PIN_SET))
#define SW_I2C_SCL_NULL  (HAL_GPIO_WritePin(SW_I2C_PORT, SW_I2C_SCL_PIN , GPIO_PIN_RESET))


#define SW_I2C_DELAY  asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop")


#define SW_I2C_ACK   0
#define SW_I2C_NACK  1


//void i2c_init(void);
//void i2c_start(void);
void i2c_stop(void);
void i2c_send_byte(uint8_t data);
uint8_t i2c_read_byte(uint8_t ack);

//void i2c_send_data16(uint16_t data);
void i2c_start_addr(uint8_t addr);



#endif  //end of I2C_H_


