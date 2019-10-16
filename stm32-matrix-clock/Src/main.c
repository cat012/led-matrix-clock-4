/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

// stm32f030
// codepage: win-1251
// 11-Oct-2019

/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <string.h>

#include "at24c32.h"
#include "max7219.h"
#include "ds3231.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define BOARD_LED_OFF     (HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET))
#define BOARD_LED_ON      (HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET))
#define BOARD_LED_TOGGLE  (HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_4))

#define TMR3_OVF_FREQ    100U
#define TMR3_PERIOD_MS   (1000U/TMR3_OVF_FREQ)
#define EVENT_PERIOD(x)  ((x)/TMR3_PERIOD_MS)

#define BRIGHTNESS_EE_ADDR  0
#define SCREENMODE_EE_ADDR  1

#define BUTTON_1  (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_3))

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim3;

/* USER CODE BEGIN PV */

uint8_t rtcdata[RTC_DATA_SIZE]={0};  //ds3231 rtc data

uint8_t newrtcdata[RTC_DATA_SIZE]={0};

char strbuff[16]={'\0'};  //string buffer

char strbuffsum[64]={'\0'};;  //string buffer

uint8_t strbufflen=0;

uint16_t scrpos=0xffff;

uint8_t scrbright=0;
uint8_t scrmode=0;

volatile uint8_t scrcnt=0;

volatile uint8_t btncnt=0;
volatile uint8_t hldcnt=0;


const char menustr[9][5]=
  {
  "MODE",
  "BRIG",
  "HOU",
  "MIN",
  "SEC",
  "DAY",
  "DAT",
  "MON",
  "YEA"
  };


const char dayname[2][7][12]=
  {
    {
    "MONDAY",
    "TUESDAY",
    "WEDNESDAY",
    "THURSDAY",
    "FRIDAY",
    "SATURDAY",
    "SUNDAY"
    },
    {
    "ПОНЕДЕЛЬНИК",
    "ВТОРНИК",
    "СРЕДА",
    "ЧЕТВЕРГ",
    "ПЯТНИЦА",
    "СУББОТА",
    "ВОСКРЕСЕНЬЕ"
    }
  };


const char monthname[2][12][10]=
  {
    {
    "JANUARY",
    "FEBRUARY",
    "MARCH",
    "APRIL",
    "MAY",
    "JUNE",
    "JULY",
    "AUGUST",
    "SEPTEMBER",
    "OCTOBER",
    "NOVEMBER",
    "DECEMBER"
    },
    {
    "ЯНВАРЬ",
    "ФЕВРАЛЬ",
    "МАРТ",
    "АПРЕЛЬ",
    "МАЙ",
    "ИЮНЬ",
    "ИЮЛЬ",
    "АВГУСТ",
    "СЕНТЯБРЬ",
    "ОКТЯБРЬ",
    "НОЯБРЬ",
    "ДЕКАБРЬ"
    }
  };

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM3_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

//=============================================================================
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim3)
    {
    if(scrcnt) scrcnt--;

    if(btncnt) btncnt--;
    if(hldcnt) hldcnt--;
    }


//-----------------------------------------------------------------------------
static uint8_t button_check(void)
    {
    uint8_t k=0;

    static uint8_t stage=0;

    switch(stage)
        {
        case 0:
            if(btncnt==0)
                {
                btncnt=EVENT_PERIOD(50);

                if(!BUTTON_1)
                    {
                    stage=1;
                    }
                }
            break;

        case 1:
            if(btncnt==0)
                {
                if(!BUTTON_1)
                    {
                    stage=2;
                    hldcnt=255;
                    }
                else stage=0;
                }
            break;

        case 2:
            if(hldcnt<(255-(uint8_t)EVENT_PERIOD(500)))
                {
                k=2;
                stage=4;
                }
            else if(btncnt==0)
                {
                btncnt=EVENT_PERIOD(50);

                if(BUTTON_1)
                    {
                    stage=3;
                    btncnt=EVENT_PERIOD(50);
                    }
                }
            break;

        case 3:
            if(btncnt==0)
                {
                if(BUTTON_1)
                    {
                    stage=0;
                    k=1;
                    }
                }
            break;

        case 4:
            if(btncnt==0)
                {
                btncnt=EVENT_PERIOD(50);

                if(!BUTTON_1)
                    {
                    stage=5;
                    btncnt=EVENT_PERIOD(50);
                    }
                }
            break;

        case 5:
            if(btncnt==0)
                {
                if(BUTTON_1)
                    {
                    stage=0;
                    }
                }
            break;
        }

    return k;
    }


//-----------------------------------------------------------------------------
static void clock_normal_mode(void)
    {
    scrcnt=EVENT_PERIOD(500);

    rtc_read(rtcdata);

    matrix_clear_shift();

    sprintf(strbuff, "%2u", rtcdata[HOURS_REG]);
    matrix_print_shift(0,strbuff);

    matrix_char_shift(14, ':');

    sprintf(strbuff, "%02u",rtcdata[MINUTES_REG]);
    matrix_print_shift(20,strbuff);

    matrix_copy_shift(0);
    }


//-----------------------------------------------------------------------------
static void clock_shift_mode(uint8_t lang)
    {
    scrcnt=EVENT_PERIOD(30);

    if(scrpos>(((uint16_t)strbufflen*7U)+MATRIX_BUFF_SIZE))
        {
        scrpos=0;

        rtc_read(rtcdata);

        int8_t tempmsb=ds3231_read_reg(0x11);

        strbuffsum[0]=0; //"clear" the buffer

        sprintf(strbuff, "%u:%02u ", rtcdata[HOURS_REG] ,rtcdata[MINUTES_REG]);
        strcat(strbuffsum, strbuff);

        strcat(strbuffsum, dayname[lang][rtcdata[DAY_REG]-1]);

        sprintf(strbuff, " %u ", rtcdata[DATE_REG]);
        strcat(strbuffsum, strbuff);

        strcat(strbuffsum, monthname[lang][rtcdata[MONTH_REG]-1]);

        sprintf(strbuff, " 20%02u", rtcdata[YEAR_REG]);
        strcat(strbuffsum, strbuff);

        sprintf(strbuff, " %+d", tempmsb);
        strcat(strbuffsum, strbuff);

        strbufflen=strlen(strbuffsum);

        matrix_clear_shift();
        matrix_print_shift(32,strbuffsum);
        }

    matrix_copy_shift(scrpos++);
    }


//-----------------------------------------------------------------------------
static void clock_compact_mode(void)
    {
    scrcnt=EVENT_PERIOD(200);

    rtc_read(rtcdata);

    sprintf(strbuff, "%02u:%02u:%02u", rtcdata[HOURS_REG], rtcdata[MINUTES_REG], rtcdata[SECONDS_REG]);
    matrix_print_small(0,strbuff);
    }


//-----------------------------------------------------------------------------
static void clock_settings_mode(uint8_t mode)
    {
    if(mode==5) scrcnt=EVENT_PERIOD(200);
    else scrcnt=EVENT_PERIOD(500);

    rtc_read(rtcdata);

    matrix_clear_shift();

    matrix_print_shift_compact(0,menustr[mode-1]);

    uint8_t t=0;

    if(mode==1) t=scrmode+1;
    if(mode==2) t=scrbright+1;
    if(mode==3) t=rtcdata[HOURS_REG];
    if(mode==4) t=rtcdata[MINUTES_REG];
    if(mode==5) t=rtcdata[SECONDS_REG];
    if(mode==6) t=rtcdata[DAY_REG];
    if(mode==7) t=rtcdata[DATE_REG];
    if(mode==8) t=rtcdata[MONTH_REG];
    if(mode==9) t=rtcdata[YEAR_REG];

    if(mode==1 || mode==2)
        {
        sprintf(strbuff, "%1u", t);
        matrix_print_shift(32-5,strbuff);
        }
    else
        {
        sprintf(strbuff, "%02u", t);
        matrix_print_shift((32-(5+5+2)),strbuff);
        }

    matrix_copy_shift(0);
    }


//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */
  

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */

  HAL_TIM_Base_Start_IT(&htim3);

  HAL_Delay(100);  //delay for matrix power up
  matrix_init();

  uint8_t setm=0;

  i2c_init();
  ds3231_init();

  scrbright=ee_read(BRIGHTNESS_EE_ADDR)&0b00000111;
  uint8_t oldbright=scrbright;

  matrix_brightness(scrbright);

  scrmode=ee_read(SCREENMODE_EE_ADDR)&0b00000011;
  uint8_t oldscrmode=scrmode;

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

    if(scrcnt==0)
        {
        if(setm==0)
            {
            if(scrmode==0) clock_normal_mode();
            if(scrmode==1) clock_shift_mode(0);
            if(scrmode==2) clock_shift_mode(1);
            if(scrmode==3) clock_compact_mode();
            }
        else clock_settings_mode(setm);

        matrix_update();
        }

    switch(button_check())
        {
        case 2:
            scrcnt=0;
            scrpos=0xffff;

            if(setm==1) { if(scrmode!=oldscrmode) { oldscrmode=scrmode; ee_write(SCREENMODE_EE_ADDR,scrmode); setm=0; break; } }
            if(setm==2) { if(scrbright!=oldbright) { oldbright=scrbright; ee_write(BRIGHTNESS_EE_ADDR,scrbright); setm=0; break; } }
            if(++setm>9) setm=0;
            break;

        case 1:
            if(setm==0) { break; }

            scrcnt=0;
            scrpos=0xffff;

            if(setm==1) { uint8_t tmp=scrmode; if(++tmp>3) tmp=0; scrmode=tmp; }
            if(setm==2) { uint8_t tmp=scrbright; if(++tmp>7) tmp=0; matrix_brightness(tmp); scrbright=tmp; }
            if(setm==3) { uint8_t tmp=rtcdata[HOURS_REG]; if(++tmp>23) tmp=0; rtc_set_hrs(tmp); }
            if(setm==4) { uint8_t tmp=rtcdata[MINUTES_REG]; if(++tmp>59) tmp=0; rtc_set_min(tmp); }
            if(setm==5) { rtc_set_sec(0); }
            if(setm==6) { uint8_t tmp=rtcdata[DAY_REG]; if(++tmp>7) tmp=1; rtc_set_day(tmp); }
            if(setm==7) { uint8_t tmp=rtcdata[DATE_REG]; if(++tmp>31) tmp=1; rtc_set_dat(tmp); }
            if(setm==8) { uint8_t tmp=rtcdata[MONTH_REG]; if(++tmp>12) tmp=1; rtc_set_mon(tmp); }
            if(setm==9) { uint8_t tmp=rtcdata[YEAR_REG]; if(++tmp>99) tmp=0; rtc_set_year(tmp); }
            break;

        case 0: break;
        default: break;
        }
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 799;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 99;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);

  /*Configure GPIO pin : PA3 */
  GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PA4 */
  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PA5 PA6 PA7 PA9 
                           PA10 */
  GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_9 
                          |GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(char *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
