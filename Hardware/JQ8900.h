#ifndef __JQ8900_H
#define __JQ8900_H

#include "stm32f10x.h"

void JQ8900_Init(void);
void JQ8900_PlayIndex(uint16_t index);
void JQ8900_SetVolume(uint8_t vol);
void JQ8900_Play(void);
void JQ8900_Pause(void);
void JQ8900_Stop(void);

#endif
