#ifndef __UART3_H_
#define __UART3_H_

#include "stm32f10x.h"

typedef struct{
	uint8_t buff[100];
	uint8_t flag;
	uint8_t len;
}TypeUsart3;
extern TypeUsart3 MyUsart3;

extern uint8_t GM65_RxBuff[256];
extern uint8_t GM65_RxLen;
extern uint8_t GM65_RxFlag;
extern volatile uint8_t USART3_RxAny;

void canmv_Receive_Data(uint8_t Com_Data);
void USART3_init(u32 baud);
void usatr3_send_data(u8 c);
void uatr3_send_data(char * data,u8 len);

#endif
