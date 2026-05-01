#include "stm32f10x.h"                  // Device header
#include "PWM.h"
#include "Delay.h"

void robot_Init(void)
{
	PWM_Init(); 
}

void robot_speed(uint8_t left1_speed,uint8_t left2_speed,uint8_t right1_speed,uint8_t right2_speed)
{	
	    TIM_SetCompare1(TIM4,left1_speed);
      TIM_SetCompare2(TIM4,left2_speed);
      TIM_SetCompare3(TIM4,right1_speed);
      TIM_SetCompare4(TIM4,right2_speed);
}

void makerobo_run(int8_t speed,uint16_t time)
{
      if(speed > 100)
			{
				speed = 100;
			}
			if(speed < 0)
			{
				speed = 0;
			}
	    robot_speed(speed,0,speed,0);
			Delay_ms(time);
}

void makerobo_brake(uint16_t time)
{
		robot_speed(0,0,0,0);
		Delay_ms(time);
}

void makerobo_Left(int8_t speed,uint16_t time)
{
	    if(speed > 100)
			{
				speed = 100;
			}
			if(speed < 0)
			{
				speed = 0;
			}
		robot_speed(0,0,speed,0);
		Delay_ms(time);
}

void makerobo_Spin_Left(int8_t speed,uint16_t time)
{
		  if(speed > 100)
			{
				speed = 100;
			}
			if(speed < 0)
			{
				speed = 0;
			}  
		robot_speed(0,speed,speed,0);
		Delay_ms(time);
}

void makerobo_Right(int8_t speed,uint16_t time)
{
	    if(speed > 100)
			{
				speed = 100;
			}
			if(speed < 0)
			{
				speed = 0;
			}
		robot_speed(speed,0,0,0);
		Delay_ms(time);
}

void makerobo_Spin_Right(int8_t speed,uint16_t time)
{
		  if(speed > 100)
			{
				speed = 100;
			}
			if(speed < 0)
			{
				speed = 0;
			}  
		robot_speed(speed,0,0,speed);
		Delay_ms(time);
}

void makerobo_back(int8_t speed,uint16_t time)
{
      if(speed > 100)
			{
				speed = 100;
			}
			if(speed < 0)
			{
				speed = 0;
			}
	    robot_speed(0,speed,0,speed);
			Delay_ms(time);
}

void makerobo_ramp(uint8_t target, uint16_t ramp_ms)
{
	uint8_t step_delay;
	uint8_t i;
	if(target > 100) target = 100;
	if(target == 0) return;
	step_delay = ramp_ms / target;
	if(step_delay < 1) step_delay = 1;
	for(i = 1; i <= target; i++)
	{
		robot_speed(i, 0, i, 0);
		Delay_ms(step_delay);
	}
}
