#ifndef __TUBE_H
#define __TUBE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

//TM_DIO -> PB9  TM_CLK -> PB8
#define TM1637_DIO_IN()     {GPIOB->CRH&=0XFFFFFF0F;GPIOB->CRH|=0X80;}
#define TM1637_DIO_OUT()    {GPIOB->CRH&=0XFFFFFF0F;GPIOB->CRH|=0X30;}

void ALIX_Delay_init(void);
void ALIX_Delay_us(uint32_t nus);
void TMserialstart(void);
void TMserialstop(void);
void TMserialask(void);
void TMserialWrByte(uint8_t oneByte);
void TMSegDisplay(uint8_t *DISCODE);

#ifdef __cplusplus
}
#endif

#endif
