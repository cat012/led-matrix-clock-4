// i2c.h
//
// Software I2C
//
// 26-05-2019
//


#ifndef I2C_H_
#define I2C_H_


#include "stm32f0xx_hal.h"


#define I2C_PORT     GPIOA
#define I2C_SDA_PIN  GPIO_PIN_10
#define I2C_SCL_PIN  GPIO_PIN_9


#define SDA_PULL  (HAL_GPIO_WritePin(I2C_PORT, I2C_SDA_PIN , GPIO_PIN_SET))
#define SDA_NULL  (HAL_GPIO_WritePin(I2C_PORT, I2C_SDA_PIN , GPIO_PIN_RESET))
#define SDA_STAT  (HAL_GPIO_ReadPin(I2C_PORT, I2C_SDA_PIN ))

#define SCL_PULL  (HAL_GPIO_WritePin(I2C_PORT, I2C_SCL_PIN , GPIO_PIN_SET))
#define SCL_NULL  (HAL_GPIO_WritePin(I2C_PORT, I2C_SCL_PIN , GPIO_PIN_RESET))


#define I2C_DELAY  asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop")


#define I2C_ACK   0
#define I2C_NACK  1


void i2c_init(void);
void i2c_start(void);
void i2c_stop(void);
void i2c_send_byte(uint8_t data);
uint8_t i2c_read_byte(uint8_t ack);



#endif  //end of I2C_H_


