/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "tube.h"
#include "ws2812.h"
#include "ds18b20.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t num[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};	//0-9
uint8_t num1[]={0xbf,0x86,0xdb,0xcf,0xe6,0xed,0xfd,0x87,0xff,0xef};	//0:-9:
uint8_t tube[4] = {0};
int8_t hour = 0, minute = 0, second = 0;

uint8_t KEY_FLAG = 0;

uint8_t index = 0;
uint16_t temperature = 0;
uint32_t colors[] = {0x1725e8,0x178ee8,0x17e8d1,0x17e820,0x76e817,0xe8e717,0xe87617,0xe81917};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void clock_display(void);
void temp_display(void);
void my_tick(void);
void disdate(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  /* USER CODE BEGIN 2 */
  ws281x_Init();
  ALIX_Delay_init();
  while(DS18B20_Init());
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  if(KEY_FLAG == 0) clock_display();
	  else disdate();
	  temp_display();
	  my_tick();
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void clock_display(void)
{
	tube[3] = num[minute % 10];
	tube[2] = num[minute / 10 % 10];
	if(second % 2 == 0){
		tube[1] = num1[hour % 10];
	} else {
		tube[1] = num[hour % 10];
	}
	tube[0] = num[hour / 10 % 10];
	TMSegDisplay(tube);
	HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);

}

void temp_display(void)
{
	temperature = DS18B20_Get_Temp();
	index = (temperature - 200) / 20;
	if(index >= sizeof(colors) / sizeof(colors[0]))
		index = sizeof(colors) / sizeof(colors[0]) - 1;
	ws281x_Play(colors, index);
}

void my_tick(void)
{
	second++;
	if(second == 60){
		second = 0;
		minute++;
	}
	if(minute == 60){
		minute = 0;
		hour++;
	}
	if(hour == 24){
		hour = 0;
	}
	ALIX_Delay_us(1000000);
}

void disdate(void)
{
	switch(KEY_FLAG)
	{
	case 1:
		{
			if(second % 2 == 0){
				tube[1] = num[hour % 10];
				tube[0] = num[hour / 10 % 10];
			} else {
				tube[1] = 0;
				tube[0] = 0;
			}
			tube[3] = num[minute % 10];
			tube[2] = num[minute / 10 % 10];
		}
		break;
	case 2:
		{
			if(second % 2 == 0){
				tube[3] = num[minute % 10];
				tube[2] = num[minute / 10 % 10];
			} else {
				tube[3] = 0;
				tube[2] = 0;
			}
			tube[1] = num[hour % 10];
			tube[0] = num[hour / 10 % 10];
		}
		break;
	default:break;
	}
	TMSegDisplay(tube);
	HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	switch(GPIO_Pin)
	{
	case KEY1_Pin:
		if(HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin) == GPIO_PIN_SET){
			KEY_FLAG++;
			if(KEY_FLAG > 2) KEY_FLAG = 0;
		}
		break;
	case KEY2_Pin:
		if((HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin) == GPIO_PIN_SET) && (KEY_FLAG == 1)){
			hour++;
			if(hour >= 24) hour = 0;
		}
		if((HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin) == GPIO_PIN_SET) && (KEY_FLAG == 2)){
			minute++;
			if(minute >= 60) minute = 0;
		}
		break;
	case KEY3_Pin:
		if((HAL_GPIO_ReadPin(KEY3_GPIO_Port, KEY3_Pin) == GPIO_PIN_SET) && (KEY_FLAG == 1)){
			hour--;
			if(hour < 0) hour = 23;
		}
		if((HAL_GPIO_ReadPin(KEY3_GPIO_Port, KEY3_Pin) == GPIO_PIN_SET) && (KEY_FLAG == 2)){
			minute--;
			if(minute < 0) minute = 59;
		}
		break;
	case KEY4_Pin:
		if(HAL_GPIO_ReadPin(KEY4_GPIO_Port, KEY4_Pin) == GPIO_PIN_SET){
		}
		break;
	default:break;
	}

}


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
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
