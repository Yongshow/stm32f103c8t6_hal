#include "tube.h"

static uint8_t fac_us=0;

void ALIX_Delay_init()
{

	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK_DIV8);
	fac_us=SystemCoreClock/8000000;

}

void ALIX_Delay_us(uint32_t nus)
{
	uint32_t temp;
	SysTick->LOAD=nus*fac_us;
	SysTick->VAL=0x00;
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;
	SysTick->VAL =0X00;
}

void TMserialstart(void)
{
	HAL_GPIO_WritePin(TM_CLK_GPIO_Port,TM_CLK_Pin,GPIO_PIN_SET); // CLK = 1
	HAL_GPIO_WritePin(TM_DIO_GPIO_Port,TM_DIO_Pin,GPIO_PIN_SET); // DIO = 1
	ALIX_Delay_us(2);
	HAL_GPIO_WritePin(TM_DIO_GPIO_Port,TM_DIO_Pin,GPIO_PIN_RESET); // DIO = 0
}

void TMserialstop(void)
{
	HAL_GPIO_WritePin(TM_CLK_GPIO_Port,TM_CLK_Pin,GPIO_PIN_RESET); // CLK = 0
	ALIX_Delay_us(2);
	HAL_GPIO_WritePin(TM_DIO_GPIO_Port,TM_DIO_Pin,GPIO_PIN_RESET); // DIO = 0
	ALIX_Delay_us(2);
	HAL_GPIO_WritePin(TM_CLK_GPIO_Port,TM_CLK_Pin,GPIO_PIN_SET); // CLK = 1
	ALIX_Delay_us(2);
	HAL_GPIO_WritePin(TM_DIO_GPIO_Port,TM_DIO_Pin,GPIO_PIN_SET); // DIO = 1
}

void TMserialask(void)
{
	TM1637_DIO_IN();
	HAL_GPIO_WritePin(TM_CLK_GPIO_Port,TM_CLK_Pin,GPIO_PIN_RESET); // CLK = 0
	ALIX_Delay_us(5);
	while(HAL_GPIO_ReadPin(TM_DIO_GPIO_Port,TM_DIO_Pin));
	HAL_GPIO_WritePin(TM_CLK_GPIO_Port,TM_CLK_Pin,GPIO_PIN_SET); // CLK = 1
	ALIX_Delay_us(2);
	HAL_GPIO_WritePin(TM_CLK_GPIO_Port,TM_CLK_Pin,GPIO_PIN_RESET); // CLK = 0
	TM1637_DIO_OUT();
}

void TMserialWrByte(uint8_t oneByte)
{
	uint8_t i;
	for(i=0;i<8;i++)
	{
		HAL_GPIO_WritePin(TM_CLK_GPIO_Port,TM_CLK_Pin,GPIO_PIN_RESET); // CLK = 0
		if(oneByte&0x01)
		{
			HAL_GPIO_WritePin(TM_DIO_GPIO_Port,TM_DIO_Pin,GPIO_PIN_SET); // DIO = 1
		}
		else
		{
			HAL_GPIO_WritePin(TM_DIO_GPIO_Port,TM_DIO_Pin,GPIO_PIN_RESET); // DIO = 0
		}
		ALIX_Delay_us(3);
		oneByte=oneByte>>1;
		HAL_GPIO_WritePin(TM_CLK_GPIO_Port,TM_CLK_Pin,GPIO_PIN_SET); // CLK = 1
		ALIX_Delay_us(3);
	}
}

void TMSegDisplay(uint8_t *DISCODE)
{
	uint8_t i;
	TMserialstart();
	TMserialWrByte(0x40);
	TMserialask();
	TMserialstop();
	TMserialstart();
	TMserialWrByte(0xc0);
	TMserialask();
	for(i=0;i<4;i++)
	{
		TMserialWrByte(*(DISCODE+i));
		TMserialask();
	}
	TMserialstop();
	TMserialstart();
	TMserialWrByte(0x8f);
	TMserialask();
	TMserialstop();
}
