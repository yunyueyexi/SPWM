#ifndef PID_H_
#define PID_H_

#include "sys.h"

typedef struct
{
	float Set_Value;
	float Actual_Value;
	float out;
	float err;
	float last_err;
	float intergral;//积分
	float Kp;
	float Ki;
	float Kd;
	float Out_MAX;
	float Out_MIN;
	float i_band;//积分限制

}pid_typedef;

extern pid_typedef pid;

void PID_Init(void);
float PID_realize(float ActualSpeed);

#endif

