#include "stm32f10x.h"
#include "LEDSEG.h"
#include "Delay.h"
#include "robot.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "Key.h"
#include "Serial.h"
#include "Uart3.h"
#include "Irtracking.h"
#include "LineFollow.h"
#include "JQ8900.h"

#define TEAM_AUDIO_INDEX    10

static uint8_t started = 0;
static uint8_t finished = 0;
static uint8_t readyToStop = 0;
static uint16_t playedMask = 0;
static uint8_t fourCount = 0;
static uint16_t fourCooldown = 0;
static uint8_t sevenCorrected = 0;
static uint8_t stopBlackCount = 0;
static uint8_t wasBlack = 0;
static LF_Context_t lfCtx;
static IR_Data_t irData;

static void GM861S_SendCmd(uint8_t *cmd, uint8_t len)
{
	uint8_t i;
	for(i = 0; i < len; i++)
	{
		usatr3_send_data(cmd[i]);
	}
	Delay_ms(50);
}

static void GM861S_SetupUART(void)
{
	uint8_t cmd_uart[] = {0x7E, 0x00, 0x08, 0x01, 0x00, 0xD5, 0x00, 0xAB, 0xCD};
	GM861S_SendCmd(cmd_uart, sizeof(cmd_uart));

	uint8_t cmd_continuous[] = {0x7E, 0x00, 0x08, 0x01, 0x00, 0xD1, 0x02, 0xAB, 0xCD};
	GM861S_SendCmd(cmd_continuous, sizeof(cmd_continuous));

	uint8_t cmd_save[] = {0x7E, 0x00, 0x09, 0x01, 0x00, 0x00, 0x00, 0xAB, 0xCD};
	GM861S_SendCmd(cmd_save, sizeof(cmd_save));
}

static void GM861S_TriggerScan(void)
{
	uint8_t cmd[] = {0x7E, 0x00, 0x08, 0x01, 0x00, 0x02, 0x01, 0xAB, 0xCD};
	GM861S_SendCmd(cmd, sizeof(cmd));
}

static void ProcessQRCode(void)
{
	if(!GM65_RxFlag) return;

	GM65_RxFlag = 0;

	uint16_t audioIndex = 0;
	uint8_t len = GM65_RxLen;

	while(len > 0 && (GM65_RxBuff[len - 1] <= 0x20 || GM65_RxBuff[len - 1] == '\r' || GM65_RxBuff[len - 1] == '\n'))
		len--;
	GM65_RxBuff[len] = '\0';

	audioIndex = (uint16_t)atoi((char *)GM65_RxBuff);

	if(audioIndex == 0 && len > 0)
	{
		uint8_t j;
		for(j = 0; j < len; j++)
		{
			if(GM65_RxBuff[j] >= '1' && GM65_RxBuff[j] <= '9')
			{
				audioIndex = GM65_RxBuff[j] - '0';
				break;
			}
		}
	}

	if(audioIndex >= 1 && audioIndex <= 9)
	{
		if(audioIndex == 4)
		{
			if(fourCooldown == 0)
			{
				fourCount++;
				fourCooldown = 1500;
				if(fourCount == 1)
					lfCtx.pendingTurnDir = 1;
				else if(fourCount == 2)
					lfCtx.pendingTurnDir = -1;
			}
		}

		if(audioIndex == 7 && fourCount == 1 && sevenCorrected == 0)
		{
			sevenCorrected = 1;
			makerobo_brake(100);
			makerobo_Spin_Right(LF_TURN_SPEED, 500);
			{
				uint16_t spinElapsed = 0;
				IR_Data_t spinIr;
				while(spinElapsed < 2000)
				{
					IR_ReadFiltered(&spinIr);
					if(!spinIr.allBlack && (spinIr.filtered[1] == IR_BLACK || spinIr.filtered[2] == IR_BLACK))
						break;
					robot_speed(LF_TURN_SPEED, 0, 0, LF_TURN_SPEED);
					Delay_ms(5);
					spinElapsed += 5;
				}
			}
			makerobo_brake(50);
			lfCtx.state = LF_STATE_FOLLOW;
			lfCtx.lastPosition = 0;
			lfCtx.lostCounter = 0;
			lfCtx.edgeCounter = 0;
			lfCtx.pendingTurnDir = 0;
		}

		uint16_t bit = (uint16_t)(1 << (audioIndex - 1));
		if(!(playedMask & bit))
		{
			playedMask |= bit;
			Digital_Display(audioIndex);
			JQ8900_PlayIndex(audioIndex);
		}

		if(audioIndex == 9)
			readyToStop = 1;
	}
	else
	{
		Digital_Display(0);
	}
}

int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	Key_Init();
	LEDSEG_Init();
	Serial_Init();
	USART3_init(9600);
	robot_Init();
	Irtracking_Init();
	JQ8900_Init();
	LF_Init(&lfCtx);
	Delay_ms(1500);
	Digital_Display(0);

	while(!started)
	{
		if(Key_GetNum() == 1)
		{
			started = 1;
			JQ8900_PlayIndex(TEAM_AUDIO_INDEX);
			Delay_ms(2000);
		}
	}

	GM861S_SetupUART();
	Delay_ms(200);
	GM861S_TriggerScan();

	while(1)
	{
		if(finished)
		{
			makerobo_brake(0);
			continue;
		}

		if(readyToStop)
			lfCtx.noJunction = 1;

		if(fourCooldown > 0)
			fourCooldown--;

		ProcessQRCode();

		LF_Update(&lfCtx, &irData);

		if(readyToStop)
		{
			uint8_t blackCnt = 0;
			uint8_t k;
			for(k = 0; k < 4; k++)
			{
				if(irData.filtered[k] == IR_BLACK)
					blackCnt++;
			}
			if(blackCnt >= 3)
			{
				if(!wasBlack)
				{
					wasBlack = 1;
					stopBlackCount++;
					if(stopBlackCount >= 2)
					{
						makerobo_run(LF_BASE_SPEED, 1000);
						finished = 1;
						makerobo_brake(0);
						continue;
					}
				}
			}
			else
			{
				wasBlack = 0;
			}
		}

		if(MyUsart3.flag)
		{
			MyUsart3.flag = 0;
		}
	}
}

void canmv_Receive_Data(uint8_t Com_Data)
{
	uint8_t i;
	static uint8_t RxCounter1 = 0;
	static uint8_t RxBuffer1[5] = {0};
	static uint8_t RxState = 0;

	if(RxState == 0 && Com_Data == 0xAA)
	{
		RxState = 1;
		RxBuffer1[RxCounter1++] = Com_Data;
	}
	else if(RxState == 1 && Com_Data == 0xAE)
	{
		RxState = 2;
		RxBuffer1[RxCounter1++] = Com_Data;
	}
	else if(RxState == 2)
	{
		if(RxCounter1 < 5)
			RxBuffer1[RxCounter1++] = Com_Data;
		if(RxCounter1 >= 5 || Com_Data == 0x5B)
		{
			RxState = 3;
		}
	}
	else if(RxState == 3)
	{
		if(RxCounter1 > 0 && RxBuffer1[RxCounter1 - 1] == 0x5B)
		{
			RxCounter1 = 0;
			RxState = 0;
		}
		else
		{
			RxState = 0;
			RxCounter1 = 0;
			for(i = 0; i < 5; i++)
				RxBuffer1[i] = 0x00;
		}
	}
	else
	{
		RxState = 0;
		RxCounter1 = 0;
		for(i = 0; i < 5; i++)
			RxBuffer1[i] = 0x00;
	}
}
