#ifndef __LINEFOLLOW_H
#define __LINEFOLLOW_H

#include "stm32f10x.h"
#include "Irtracking.h"

#define LF_BASE_SPEED      70
#define LF_MAX_SPEED        90
#define LF_SLOW_SPEED      50
#define LF_TURN_SPEED      85

#define LF_LOST_TIMEOUT     500
#define LF_CREEP_SPEED      70
#define LF_CREEP_TRIM       12

#define LF_JUNCTION_FWD_MS  250

#define LF_SHARP_BRAKE_MS   30
#define LF_SHARP_SPIN_MAX   800
#define LF_SHARP_BLIND_MS   300
#define LF_SHARP_TRIGGER    6
#define LF_TURN_LOCK        15

typedef enum {
	LF_STATE_START,
	LF_STATE_FOLLOW,
	LF_STATE_LOST,
	LF_STATE_STOP
} LF_State_t;

typedef struct {
	LF_State_t state;
	int8_t lastPosition;
	uint16_t lostCounter;
	uint8_t edgeCounter;
	uint8_t turnLockCounter;
	uint8_t noJunction;
	int8_t pendingTurnDir;
} LF_Context_t;

void LF_Init(LF_Context_t *ctx);
void LF_Update(LF_Context_t *ctx, IR_Data_t *ir);
LF_State_t LF_GetState(LF_Context_t *ctx);

#endif
