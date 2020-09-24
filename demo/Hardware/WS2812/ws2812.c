#include "ws2812.h"
#include "tube.h"

/**
  * @brief  WS281x send bit\RESET function
  * @param
  * @retval None
  */
void ws281x_sendLow(void)
{
	HAL_GPIO_WritePin(WS_LED_GPIO_Port,WS_LED_Pin,GPIO_PIN_SET);
	__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
	HAL_GPIO_WritePin(WS_LED_GPIO_Port,WS_LED_Pin,GPIO_PIN_RESET);
	__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
	__NOP();__NOP();__NOP();__NOP();__NOP();
}
void ws281x_sendHigh(void)
{
	HAL_GPIO_WritePin(WS_LED_GPIO_Port,WS_LED_Pin,GPIO_PIN_SET);
	__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
	__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
	HAL_GPIO_WritePin(WS_LED_GPIO_Port,WS_LED_Pin,GPIO_PIN_RESET);
	__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
	__NOP();__NOP();
}

void ws281x_Init(void)
{
	HAL_GPIO_WritePin(WS_LED_GPIO_Port,WS_LED_Pin,GPIO_PIN_RESET);
	HAL_Delay(1);
}

/**
  * @brief  ws281x rgb color set function
  * @param  dat:rgb color code(24bit)
  * @retval None
  */
void ws281x_sendRGB(uint32_t rgb)
{
	uint8_t i;
	uint32_t dat = 0x110000;
	dat = ((rgb<<8)&0xff0000)+((rgb>>8)&0x00ff00)+(rgb&0x0000ff);
	for(i = 0; i<24; i++)
	{
		if(((dat<<i) & 0x800000))
			ws281x_sendHigh();
		else
			ws281x_sendLow();
	}
}

/**
  * @brief  shutdown all led
  * @param  None
  * @retval None
  */
void ws281x_shutdown(void)
{
	for(uint8_t i = 0; i < 8; i++){
		ws281x_sendRGB(0x000000);
	}
}

/**
 * @brief	display the leds
 * @param	colors[8], index
 * @return	None
 */
void ws281x_Play(uint32_t* colors, uint8_t index)
{
	uint8_t diff = 8 - index;
	for(uint8_t i = 0; i <= index; i++)
	{
		ws281x_sendRGB(*(colors + i));
	}
	for(uint8_t i = 0; i < diff; i++)
	{
		ws281x_sendRGB(0x000000);
	}

}
