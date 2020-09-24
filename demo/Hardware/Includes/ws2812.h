#ifndef __WS2812_H
#define __WS2812_H

#include "main.h"

void ws281x_Init(void);
void ws281x_Play(uint32_t* colors, uint8_t index);
void ws281x_shutdown(void);
void ws281x_sendRGB(uint32_t rgb);

#endif
