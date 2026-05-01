#include "stm32f10x.h"
#include "Irtracking.h"

void Irtracking_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

uint8_t L1_Irtracking_Get(void)
{
	return GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_14);
}

uint8_t L2_Irtracking_Get(void)
{
	return GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13);
}

uint8_t R1_Irtracking_Get(void)
{
	return GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_15);
}

uint8_t R2_Irtracking_Get(void)
{
	return GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12);
}

void IR_ReadRaw(IR_Data_t *data)
{
	data->raw[0] = L1_Irtracking_Get();
	data->raw[1] = L2_Irtracking_Get();
	data->raw[2] = R1_Irtracking_Get();
	data->raw[3] = R2_Irtracking_Get();
}

void IR_ReadFiltered(IR_Data_t *data)
{
	uint8_t counts[IR_SENSOR_COUNT] = {0};
	uint8_t i, j;

	for(i = 0; i < IR_FILTER_SAMPLES; i++)
	{
		counts[0] += L1_Irtracking_Get();
		counts[1] += L2_Irtracking_Get();
		counts[2] += R1_Irtracking_Get();
		counts[3] += R2_Irtracking_Get();
	}

	data->allBlack = 1;
	data->allWhite = 1;

	for(j = 0; j < IR_SENSOR_COUNT; j++)
	{
		data->filtered[j] = (counts[j] <= (IR_FILTER_SAMPLES / 2)) ? IR_BLACK : IR_WHITE;
		if(data->filtered[j] == IR_BLACK)
			data->allWhite = 0;
		else
			data->allBlack = 0;
	}
}

uint8_t IR_IsLost(IR_Data_t *data)
{
	return data->allWhite;
}

uint8_t IR_IsOnLine(IR_Data_t *data)
{
	return !data->allWhite;
}
