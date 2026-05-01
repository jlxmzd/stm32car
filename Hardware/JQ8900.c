#include "stm32f10x.h"
#include "JQ8900.h"
#include "Delay.h"

#define JQ8900_PIN   GPIO_Pin_1
#define JQ8900_PORT  GPIOB

#define JQ8900_HIGH  GPIO_SetBits(JQ8900_PORT, JQ8900_PIN)
#define JQ8900_LOW   GPIO_ResetBits(JQ8900_PORT, JQ8900_PIN)

void JQ8900_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = JQ8900_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(JQ8900_PORT, &GPIO_InitStruct);
	
	JQ8900_HIGH;
}

static void JQ8900_SendByte(uint8_t data)
{
	uint8_t i;
	
	__disable_irq();
	
	JQ8900_HIGH;
	Delay_us(1000);
	JQ8900_LOW;
	Delay_us(4000);
	
	for(i = 0; i < 8; i++)
	{
		JQ8900_HIGH;
		if(data & 0x01)
		{
			Delay_us(500);
			JQ8900_LOW;
			Delay_us(210);
		}
		else
		{
			Delay_us(210);
			JQ8900_LOW;
			Delay_us(500);
		}
		data >>= 1;
	}
	
	JQ8900_HIGH;
	
	__enable_irq();
}

void JQ8900_PlayIndex(uint16_t index)
{
	if(index >= 10)
	{
		uint8_t high = (uint8_t)(index / 10);
		uint8_t low  = (uint8_t)(index % 10);
		JQ8900_SendByte(0x0A);
		Delay_ms(5);
		JQ8900_SendByte(high);
		Delay_ms(5);
		JQ8900_SendByte(low);
		Delay_ms(5);
		JQ8900_SendByte(0x0B);
	}
	else
	{
		JQ8900_SendByte(0x0A);
		Delay_ms(5);
		JQ8900_SendByte((uint8_t)index);
		Delay_ms(5);
		JQ8900_SendByte(0x0B);
	}
}

void JQ8900_SetVolume(uint8_t vol)
{
	uint8_t i;
	if(vol > 30) vol = 30;
	for(i = 0; i < 30; i++)
	{
		JQ8900_SendByte(0x16);
		Delay_ms(20);
	}
	for(i = 0; i < vol; i++)
	{
		JQ8900_SendByte(0x15);
		Delay_ms(20);
	}
}

void JQ8900_Play(void)
{
	JQ8900_SendByte(0x11);
}

void JQ8900_Pause(void)
{
	JQ8900_SendByte(0x11);
}

void JQ8900_Stop(void)
{
	JQ8900_SendByte(0x12);
}
