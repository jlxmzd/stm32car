#ifndef __IRTRACKING_H
#define __IRTRACKING_H

#include "stm32f10x.h"

#define IR_SENSOR_COUNT   4
#define IR_FILTER_SAMPLES 5

#define IR_BLACK  0
#define IR_WHITE  1

typedef struct {
	uint8_t raw[IR_SENSOR_COUNT];
	uint8_t filtered[IR_SENSOR_COUNT];
	uint8_t allBlack;
	uint8_t allWhite;
} IR_Data_t;

void Irtracking_Init(void);

uint8_t L1_Irtracking_Get(void);
uint8_t L2_Irtracking_Get(void);
uint8_t R1_Irtracking_Get(void);
uint8_t R2_Irtracking_Get(void);

void IR_ReadRaw(IR_Data_t *data);
void IR_ReadFiltered(IR_Data_t *data);
uint8_t IR_IsLost(IR_Data_t *data);
uint8_t IR_IsOnLine(IR_Data_t *data);

#endif
