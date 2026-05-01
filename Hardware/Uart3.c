#include "stm32f10x.h"
#include "Uart3.h"
#include <string.h>
#include <stdio.h>

TypeUsart3 MyUsart3;

uint8_t GM65_RxBuff[256];
uint8_t GM65_RxLen = 0;
uint8_t GM65_RxFlag = 0;
volatile uint8_t USART3_RxAny = 0;

void USART3_init(uint32_t baud)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	USART_InitTypeDef USART_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	USART_InitStruct.USART_BaudRate = baud;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(USART3, &USART_InitStruct);

	USART_Cmd(USART3, ENABLE);
	USART_ClearFlag(USART3, USART_FLAG_TC);

	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);

	NVIC_InitStruct.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);

	USART_ReceiveData(USART3);
}

void USART3_IRQHandler(void)
{
	uint8_t incomingByte;

	if(USART_GetITStatus(USART3, USART_IT_RXNE) == SET)
	{
		USART_ClearITPendingBit(USART3, USART_IT_RXNE);
		incomingByte = USART_ReceiveData(USART3);
		if(MyUsart3.len < 99)
		{
			MyUsart3.buff[MyUsart3.len++] = incomingByte;
		}
		USART3_RxAny = 1;
	}
	if(USART_GetITStatus(USART3, USART_IT_IDLE) == SET)
	{
		USART_ReceiveData(USART3);
		MyUsart3.buff[MyUsart3.len] = '\0';
		MyUsart3.flag = 1;

		if(GM65_RxFlag == 0 && MyUsart3.len > 0)
		{
			memcpy(GM65_RxBuff, MyUsart3.buff, MyUsart3.len);
			GM65_RxBuff[MyUsart3.len] = '\0';
			GM65_RxLen = MyUsart3.len;
			GM65_RxFlag = 1;
		}

		MyUsart3.len = 0;
	}
}

void usatr3_send_data(uint8_t c)
{
	while(!USART_GetFlagStatus(USART3, USART_FLAG_TXE));
	USART_SendData(USART3, c);
	while(!USART_GetFlagStatus(USART3, USART_FLAG_TC));
}

void uatr3_send_data(char *data, uint8_t len)
{
	while(len--)
	{
		usatr3_send_data(*data);
		data++;
	}
}
