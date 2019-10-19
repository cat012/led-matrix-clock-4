// at24c32.h
//
// AT24C32 4096*8 EEPROM
//
// 17-10-2019
//


#ifndef AT24C32_H_
#define AT24C32_H_



#include <stdint.h>

#include "main.h"

#include "i2c.h"


#define AT24C32_I2C_WRITE  0b10101110
#define AT24C32_I2C_READ   0b10101111


#define AT24C32_DELAY_WRITE  HAL_Delay(10)


void ee_write(uint16_t addr, uint8_t data);
uint8_t ee_read(uint16_t addr);
void ee_clear(void);



#endif // AT24C32_H_



