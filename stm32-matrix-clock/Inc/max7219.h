// LED Dot Matrix
//
// file: max7219.h
//
//-----------------
//   Y
// X 0-31
//   |
//   7
//-----------------
//
// 16-10-2019


#ifndef MAX7219_H_
#define MAX7219_H_



#include <stdint.h>

#include "stm32f0xx_hal.h"



/////////////////////////////////////////////////////////////////////

#define MAX7219_PORT  GPIOA

#define MAX7219_CS   GPIO_PIN_6
#define MAX7219_CLK  GPIO_PIN_5
#define MAX7219_DIN  GPIO_PIN_7

/////////////////////////////////////////////////////////////////////

#define MAX7219_CS_PIN(s)    (HAL_GPIO_WritePin(MAX7219_PORT, MAX7219_CS, (s)))
#define MAX7219_CLK_PIN(s)   (HAL_GPIO_WritePin(MAX7219_PORT, MAX7219_CLK, (s)))
#define MAX7219_DIN_PIN(s)   (HAL_GPIO_WritePin(MAX7219_PORT, MAX7219_DIN, (s)))



extern const uint8_t font5x8[];
extern const uint8_t font3x7[];


void matrix_init(void);

void matrix_brightness(uint8_t val);

void matrix_clear(void);
void matrix_fill(void);

void matrix_update(void);

void matrix_pixel(uint8_t x, uint8_t y, uint8_t on);

void matrix_char_small(uint8_t xpos, uint8_t code);
void matrix_print_small(uint8_t xpos, const char *str);

void matrix_clear_shift(void);
void matrix_copy_shift(uint16_t cpos);
void matrix_pixel_shift(uint16_t xpos, uint8_t ypos, uint8_t on);
void matrix_char_shift(uint16_t xpos, uint8_t code);
void matrix_print_shift(uint16_t xpos, const char *str);
void matrix_print_shift_compact(uint8_t xpos, const char *str);


#define MAX7219_NUM  4U

#define MATRIX_BUFF_SIZE  (8U*MAX7219_NUM)

#define MATRIX_SHIFT_BUFF_SIZE  (16U*MATRIX_BUFF_SIZE)


#define MAX7219_MAP_INTENSITY   0x0a
#define MAX7219_INTENSITY_MASK  0b00001111

#define MAX7219_MAP_SHUTDOWN    0x0c
#define MAX7219_MAP_DECODEMODE  0x09
#define MAX7219_MAP_SCANLIMIT   0x0b
#define MAX7219_MAP_DISPTEST    0x0f



#define REG_BIT_SET(reg, bit) ((reg) |= (1<<(bit)))
#define REG_BIT_CLR(reg, bit) ((reg) &= ~(1<<(bit)))

#define REG_BIT_IS_SET(reg, bit) ((reg)>>(bit)&1)
#define REG_BIT_IS_CLR(reg, bit) (!((reg)>>(bit)&1))



#endif /* MAX7219_H_ */


