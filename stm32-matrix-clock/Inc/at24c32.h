// at24c32.h
//
// AT24C32 4096*8 EEPROM
//
// (^^)~  23-05-2019
//


#ifndef AT24C32_H_
#define AT24C32_H_



#include <stdint.h>

#include "main.h"

#include "i2c.h"


#define EE_W  0b10101110
#define EE_R  0b10101111


#define EE_DELAY_WRITE  HAL_Delay(10)

void ee_write(uint16_t addr, uint8_t data);
uint8_t ee_read(uint16_t addr);
void ee_clear(void);



#endif // AT24C32_H_



