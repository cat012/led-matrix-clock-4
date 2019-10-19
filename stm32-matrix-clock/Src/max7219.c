// file: max7219.c
//
// 16-10-2019


#include "max7219.h"


uint8_t matrixbuff[MATRIX_BUFF_SIZE]={0};

uint8_t matrixtempbuff[MAX7219_NUM*2]={0};

uint8_t matrixshiftbuff[MATRIX_SHIFT_BUFF_SIZE]={0};



//-------------------------------------------------------------------------------------------------
static inline void max7219_gpio_init(void)
    {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    HAL_GPIO_WritePin(MAX7219_PORT, MAX7219_CS | MAX7219_CLK | MAX7219_DIN, GPIO_PIN_RESET);
    GPIO_InitStruct.Pin = MAX7219_CS | MAX7219_CLK | MAX7219_DIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(MAX7219_PORT, &GPIO_InitStruct);
    }


//-------------------------------------------------------------------------------------------------
static void matrix_spi_write(uint8_t *data)
    {
    MAX7219_CS_PIN(GPIO_PIN_RESET);

    for(uint8_t i=0; i<MAX7219_NUM*2; i++)
        {
        for(uint8_t mask=0x80; mask; mask>>=1)
            {
            (data[i] & mask) ? MAX7219_DIN_PIN(GPIO_PIN_SET) : MAX7219_DIN_PIN(GPIO_PIN_RESET); //data line
            MAX7219_CLK_PIN(GPIO_PIN_SET);; //clock pulse
            MAX7219_CLK_PIN(GPIO_PIN_RESET);;
            }
        }

    MAX7219_CS_PIN(GPIO_PIN_SET);
    }


//-------------------------------------------------------------------------------------------------
static void matrix_command(uint8_t addr, uint8_t data)
    {
    for(uint8_t i=0; i<MAX7219_NUM*2; i+=2)
        {
        matrixtempbuff[i]=addr;
        matrixtempbuff[i+1]=data;
        }
    matrix_spi_write(matrixtempbuff);
    }


//-------------------------------------------------------------------------------------------------
static void matrix_data(uint8_t *data)
    {
    for(uint8_t k=0; k<8; k++)
        {
        for(uint8_t i=0; i<MAX7219_NUM*2; i+=2)
            {
            matrixtempbuff[i]=1+k;
            matrixtempbuff[i+1]=*data++;
            }
        matrix_spi_write(matrixtempbuff);
        }
    }


//------------------------------------------------------------------------------------------------
void matrix_clear(void)
    {
    for(uint8_t i=0; i<MATRIX_BUFF_SIZE; i++) matrixbuff[i]=0;
    }


//------------------------------------------------------------------------------------------------
void matrix_fill(void)
    {
    for(uint8_t i=0; i<MATRIX_BUFF_SIZE; i++) matrixbuff[i]=0xff;
    }


//------------------------------------------------------------------------------------------------
void matrix_brightness(uint8_t val)
    {
    matrix_command(MAX7219_MAP_INTENSITY, (val & MAX7219_INTENSITY_MASK));
    }


//-------------------------------------------------------------------------------------------------
void matrix_init(void)
    {
    max7219_gpio_init();

    matrix_command(MAX7219_MAP_SHUTDOWN, 0x00);    //shutdown mode
    matrix_command(MAX7219_MAP_DECODEMODE, 0x00);  //no decode
    matrix_command(MAX7219_MAP_INTENSITY, 0x00);   //minimum brightness
    matrix_command(MAX7219_MAP_SCANLIMIT, 0x07);   //display all digits
    matrix_command(MAX7219_MAP_DISPTEST, 0x00);    //normal operation

    matrix_clear();
    matrix_data(matrixbuff);

    matrix_command(MAX7219_MAP_SHUTDOWN, 0x01);      //normal operation
    }


//------------------------------------------------------------------------------------------------
void matrix_update(void)
    {
    matrix_data(matrixbuff);
    matrix_clear();
    }


//-------------------------------------------------------------------------------------------------
void matrix_pixel(uint8_t x, uint8_t y, uint8_t on)
    {
    if(on) (REG_BIT_SET(matrixbuff[((x%MATRIX_BUFF_SIZE)+((y%8)*MATRIX_BUFF_SIZE))/8],7-(x%8)));
    else   (REG_BIT_CLR(matrixbuff[((x%MATRIX_BUFF_SIZE)+((y%8)*MATRIX_BUFF_SIZE))/8],7-(x%8)));
    }


/*
//-------------------------------------------------------------------------------------------------
void matrix_pixel(uint8_t x, uint8_t y, uint8_t on)
    {
    if((y<=7) && (x<=(MATRIX_BUFF_SIZE-1)))
        {
        if(on) (REG_BIT_SET(matrixbuff[(x+(y*MATRIX_BUFF_SIZE))/8],7-(x%8)));
        else   (REG_BIT_CLR(matrixbuff[(x+(y*MATRIX_BUFF_SIZE))/8],7-(x%8)));
        }
    }
 */


//-------------------------------------------------------------------------------------------------
void matrix_char_small(uint8_t xpos, uint8_t code)
    {
    uint8_t c=0;

    if(code>=48 && code<=58) c=code-48;
    else if(code==32) c=11;
    else if(code==43) c=12;
    else if(code==45) c=13;
    else c=14;

    for(uint8_t y=0; y<7; y++)
        {
        for(uint8_t x=0; x<3; x++)
            {
        	if(REG_BIT_IS_SET(font3x7[c*8+y],(2-x))) matrix_pixel((uint8_t)(x+xpos),y,1);
            }
        }
    }


//-------------------------------------------------------------------------------------------------
void matrix_print_small(uint8_t xpos, const char *str)
    {
    for(; *str; xpos+=4) matrix_char_small(xpos,*str++);
    }


//------------------------------------------------------------------------------------------------
void matrix_clear_shift(void)
    {
    for(uint16_t i=0; i<MATRIX_SHIFT_BUFF_SIZE; i++) matrixshiftbuff[i]=0x00;
    }


//------------------------------------------------------------------------------------------------
void matrix_copy_shift(uint16_t cpos)
    {
    if(cpos>((MATRIX_SHIFT_BUFF_SIZE-MATRIX_BUFF_SIZE)-1)) cpos=0;

    for(uint8_t x=0; x<MATRIX_BUFF_SIZE; x++)
        {
        for(uint8_t y=0; y<8; y++)
            {
            if(REG_BIT_IS_SET(matrixshiftbuff[x+cpos],y)) matrix_pixel(x,y,1);
            }
        }
    }


//------------------------------------------------------------------------------------------------
void matrix_pixel_shift(uint16_t xpos, uint8_t ypos, uint8_t on)
    {
    if((ypos<=7) && (xpos<=(MATRIX_SHIFT_BUFF_SIZE-1)))
        {
        uint16_t x = xpos;
        uint8_t y = ypos;
        if(on) REG_BIT_SET(matrixshiftbuff[x],y);
        else REG_BIT_CLR(matrixshiftbuff[x],y);
        }
    }


//------------------------------------------------------------------------------------------------
void matrix_char_shift(uint16_t xpos, uint8_t code)
    {
    uint8_t c=0;

    if(code>=48 && code<=58) c=code-48;
    else if(code>=65 && code<=90) c=(code-65)+15;
    else if(code>=192 && code<=223) c=(code-192)+42;
    else if(code==32) c=11;
    else if(code==43) c=12;
    else if(code==45) c=13;
    else if(code==46) c=14;
    else c=41;

    for(uint8_t y=0; y<8; y++)
        {
        for(uint8_t x=0; x<5; x++)
            {
            if(REG_BIT_IS_SET(font5x8[c*8+y],(4-x))) matrix_pixel_shift(x+xpos,y,1);
            }
        }
    }


//------------------------------------------------------------------------------------------------
void matrix_print_shift(uint16_t xpos, const char *str)
    {
    for(; *str; xpos+=7) matrix_char_shift(xpos,*str++);
    }


//-------------------------------------------------------------------------------------------------
void matrix_print_shift_compact(uint8_t xpos, const char *str)
    {
    for(; *str; xpos+=6) matrix_char_shift(xpos,*str++);
    }


