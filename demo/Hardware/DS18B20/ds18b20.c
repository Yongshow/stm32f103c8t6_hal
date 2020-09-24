#include "ds18b20.h"
#include "tube.h"

//复位DS18B20
void DS18B20_Rst(void)
{
	DS18B20_IO_OUT(); 	//SET PB10 OUTPUT
    HAL_GPIO_WritePin(DS18B20_GPIO_Port, DS18B20_Pin, GPIO_PIN_RESET); 	//拉低DQ
    ALIX_Delay_us(750);    	//拉低750us
    HAL_GPIO_WritePin(DS18B20_GPIO_Port, DS18B20_Pin, GPIO_PIN_SET); 	//DQ=1
	ALIX_Delay_us(15);     	//15US
}
//等待DS18B20的回应
//返回1:未检测到DS18B20的存在
//返回0:存在
uint8_t DS18B20_Check(void)
{
	uint8_t retry=0;
	DS18B20_IO_IN();	//SET PB10 INPUT
    while (HAL_GPIO_ReadPin(DS18B20_GPIO_Port, DS18B20_Pin)&&retry<200)
	{
		retry++;
		ALIX_Delay_us(1);
	};
	if(retry>=200)return 1;
	else retry=0;
    while (!HAL_GPIO_ReadPin(DS18B20_GPIO_Port, DS18B20_Pin)&&retry<240)
	{
		retry++;
		ALIX_Delay_us(1);
	};
	if(retry>=240)return 1;
	return 0;
}
//从DS18B20读取一个位
//返回值：1/0
uint8_t DS18B20_Read_Bit(void)
{
    uint8_t data;
	DS18B20_IO_OUT();	//SET PB10 OUTPUT
	HAL_GPIO_WritePin(DS18B20_GPIO_Port, DS18B20_Pin, GPIO_PIN_RESET);
	ALIX_Delay_us(2);
	HAL_GPIO_WritePin(DS18B20_GPIO_Port, DS18B20_Pin, GPIO_PIN_SET);
	DS18B20_IO_IN();	//SET PB10 INPUT
	ALIX_Delay_us(12);
	if(HAL_GPIO_ReadPin(DS18B20_GPIO_Port, DS18B20_Pin)) data=1;
    else data=0;
    ALIX_Delay_us(50);
    return data;
}
//从DS18B20读取一个字节
//返回值：读到的数据
uint8_t DS18B20_Read_Byte(void)
{
    uint8_t i,j,dat;
    dat=0;
	for (i=1;i<=8;i++)
	{
        j=DS18B20_Read_Bit();
        dat=(j<<7)|(dat>>1);
    }
    return dat;
}
//写一个字节到DS18B20
//dat：要写入的字节
void DS18B20_Write_Byte(uint8_t dat)
 {
	uint8_t j;
	uint8_t testb;
	DS18B20_IO_OUT();	//SET Pb10 OUTPUT;
    for (j=1;j<=8;j++)
	{
        testb=dat&0x01;
        dat=dat>>1;
        if (testb)
        {
        	HAL_GPIO_WritePin(DS18B20_GPIO_Port, DS18B20_Pin, GPIO_PIN_RESET);	// Write 1
            ALIX_Delay_us(2);
            HAL_GPIO_WritePin(DS18B20_GPIO_Port, DS18B20_Pin, GPIO_PIN_SET);
            ALIX_Delay_us(60);
        }
        else
        {
        	HAL_GPIO_WritePin(DS18B20_GPIO_Port, DS18B20_Pin, GPIO_PIN_RESET);	// Write 0
            ALIX_Delay_us(60);
            HAL_GPIO_WritePin(DS18B20_GPIO_Port, DS18B20_Pin, GPIO_PIN_SET);
            ALIX_Delay_us(2);
        }
    }
}
//开始温度转换
void DS18B20_Start(void)
{
    DS18B20_Rst();
	DS18B20_Check();
    DS18B20_Write_Byte(0xcc);	// skip rom
    DS18B20_Write_Byte(0x44);	// convert
}

//初始化DS18B20的IO口 DQ 同时检测DS的存在
//返回1:不存在
//返回0:存在
uint8_t DS18B20_Init(void)
{
	DS18B20_Rst();
	return DS18B20_Check();
}
//从ds18b20得到温度值
//精度：0.1C
//返回值：温度值 （-550~1250）
short DS18B20_Get_Temp(void)
{
    uint8_t temp;
    uint8_t TL,TH;
	short tem;
    DS18B20_Start ();  			// ds1820 start convert
    DS18B20_Rst();
    DS18B20_Check();
    DS18B20_Write_Byte(0xcc);	// skip rom
    DS18B20_Write_Byte(0xbe);	// convert
    TL=DS18B20_Read_Byte(); 	// LSB
    TH=DS18B20_Read_Byte(); 	// MSB

    if(TH>7)
    {
        TH=~TH;
        TL=~TL;
        temp=0;					//温度为负
    }else temp=1;				//温度为正
    tem=TH; 					//获得高八位
    tem<<=8;
    tem+=TL;					//获得底八位
    tem=(float)tem*0.625;		//转换
	if(temp)return tem; 		//返回温度值
	else return -tem;
}
