#include "stm32f10x.h"
#include "LineFollow.h"
#include "Irtracking.h"
#include "robot.h"
#include "Delay.h"

static void LF_SharpTurn(LF_Context_t *ctx, int8_t dir)
{
	uint16_t elapsed = 0;
	IR_Data_t tmp;

	makerobo_brake(LF_SHARP_BRAKE_MS);

	if(dir < 0)
		robot_speed(0, 0, LF_TURN_SPEED, 0);
	else
		robot_speed(LF_TURN_SPEED, 0, 0, 0);
	Delay_ms(LF_SHARP_BLIND_MS);

	elapsed = LF_SHARP_BLIND_MS;

	while(elapsed < LF_SHARP_SPIN_MAX)
	{
		IR_ReadFiltered(&tmp);
		if(!tmp.allBlack && (tmp.filtered[1] == IR_BLACK || tmp.filtered[2] == IR_BLACK))
			break;
		if(dir < 0)
			robot_speed(0, 0, LF_TURN_SPEED, 0);
		else
			robot_speed(LF_TURN_SPEED, 0, 0, 0);
		Delay_ms(5);
		elapsed += 5;
	}

	makerobo_brake(0);
	ctx->lastPosition = (dir < 0) ? -1 : 1;
}

void LF_Init(LF_Context_t *ctx)
{
	ctx->state = LF_STATE_START;
	ctx->lastPosition = 0;
	ctx->lostCounter = 0;
	ctx->edgeCounter = 0;
	ctx->turnLockCounter = 0;
	ctx->noJunction = 0;
	ctx->pendingTurnDir = 0;
}

static void LF_DoFollow(LF_Context_t *ctx, IR_Data_t *ir)
{
	uint8_t L1 = (ir->filtered[0] == IR_BLACK);
	uint8_t L2 = (ir->filtered[1] == IR_BLACK);
	uint8_t R1 = (ir->filtered[2] == IR_BLACK);
	uint8_t R2 = (ir->filtered[3] == IR_BLACK);

	if(ir->allWhite)
	{
		ctx->lostCounter++;
		if(ctx->lostCounter > LF_LOST_TIMEOUT)
		{
			ctx->state = LF_STATE_LOST;
			makerobo_brake(0);
			return;
		}
		if(ctx->lastPosition > 0)
			robot_speed(LF_SLOW_SPEED, 0, 0, 0);
		else if(ctx->lastPosition < 0)
			robot_speed(0, 0, LF_SLOW_SPEED, 0);
		else
			makerobo_run(LF_CREEP_SPEED, 0);
		return;
	}

	ctx->lostCounter = 0;

	uint8_t blackCount = L1 + L2 + R1 + R2;

	if(ctx->pendingTurnDir != 0 && ir->allBlack)
	{
		int8_t dir = ctx->pendingTurnDir;
		ctx->pendingTurnDir = 0;
		makerobo_run(LF_BASE_SPEED, LF_JUNCTION_FWD_MS);
		LF_SharpTurn(ctx, dir);
		return;
	}

	if(ir->allBlack)
	{
		if(ctx->lastPosition >= 2 || ctx->lastPosition <= -2)
		{
			if(ctx->lastPosition > 0)
				robot_speed(LF_SLOW_SPEED, 0, 0, 0);
			else
				robot_speed(0, 0, LF_SLOW_SPEED, 0);
		}
		else
		{
			makerobo_run(LF_SLOW_SPEED, 0);
		}
		return;
	}

	if(ctx->turnLockCounter > 0)
	{
		ctx->turnLockCounter--;
		if(ctx->lastPosition > 0)
			robot_speed(LF_MAX_SPEED, 0, 0, 0);
		else
			robot_speed(0, 0, LF_MAX_SPEED, 0);
		return;
	}

	if((L2 || R1) && R2)
	{
		ctx->edgeCounter++;
		if(ctx->pendingTurnDir == 0 && ctx->edgeCounter >= LF_SHARP_TRIGGER)
		{
			ctx->edgeCounter = 0;
			LF_SharpTurn(ctx, 1);
		}
		else
		{
			robot_speed(LF_MAX_SPEED, 0, 0, 0);
		}
		ctx->lastPosition = 2;
		ctx->turnLockCounter = LF_TURN_LOCK;
	}
	else if(L1 && (R1 || L2))
	{
		ctx->edgeCounter++;
		if(ctx->pendingTurnDir == 0 && ctx->edgeCounter >= LF_SHARP_TRIGGER)
		{
			ctx->edgeCounter = 0;
			LF_SharpTurn(ctx, -1);
		}
		else
		{
			robot_speed(0, 0, LF_MAX_SPEED, 0);
		}
		ctx->lastPosition = -2;
		ctx->turnLockCounter = LF_TURN_LOCK;
	}
	else if(L1)
	{
		robot_speed(0, 0, LF_MAX_SPEED, 0);
		ctx->lastPosition = -3;
		ctx->edgeCounter = 0;
		ctx->turnLockCounter = LF_TURN_LOCK;
	}
	else if(R2)
	{
		robot_speed(LF_MAX_SPEED, 0, 0, 0);
		ctx->lastPosition = 3;
		ctx->edgeCounter = 0;
		ctx->turnLockCounter = LF_TURN_LOCK;
	}
	else if(L2 && !R1)
	{
		robot_speed(LF_BASE_SPEED - 10, 0, LF_BASE_SPEED + 10, 0);
		ctx->lastPosition = -1;
		ctx->edgeCounter = 0;
	}
	else if(!L2 && R1)
	{
		robot_speed(LF_BASE_SPEED + 10, 0, LF_BASE_SPEED - 10, 0);
		ctx->lastPosition = 1;
		ctx->edgeCounter = 0;
	}
	else if(L2 && R1)
	{
		makerobo_run(LF_BASE_SPEED, 0);
		ctx->lastPosition = 0;
		ctx->edgeCounter = 0;
	}
	else
	{
		makerobo_run(LF_SLOW_SPEED, 0);
		ctx->edgeCounter = 0;
	}
}

static void LF_DoLost(LF_Context_t *ctx, IR_Data_t *ir)
{
	if(ctx->lastPosition > 0)
		robot_speed(LF_SLOW_SPEED, 0, 0, 0);
	else
		robot_speed(0, 0, LF_SLOW_SPEED, 0);

	if(IR_IsOnLine(ir))
	{
		ctx->state = LF_STATE_FOLLOW;
		ctx->lostCounter = 0;
	}
}

static void LF_DoStart(LF_Context_t *ctx, IR_Data_t *ir)
{
	makerobo_ramp(LF_BASE_SPEED, 500);
	makerobo_run(LF_BASE_SPEED, 500);
	ctx->state = LF_STATE_FOLLOW;
}

void LF_Update(LF_Context_t *ctx, IR_Data_t *ir)
{
	IR_ReadFiltered(ir);

	switch(ctx->state)
	{
		case LF_STATE_START:
			LF_DoStart(ctx, ir);
			break;
		case LF_STATE_FOLLOW:
			LF_DoFollow(ctx, ir);
			break;
		case LF_STATE_LOST:
			LF_DoLost(ctx, ir);
			break;
		case LF_STATE_STOP:
			makerobo_brake(0);
			break;
		default:
			break;
	}
}

LF_State_t LF_GetState(LF_Context_t *ctx)
{
	return ctx->state;
}
