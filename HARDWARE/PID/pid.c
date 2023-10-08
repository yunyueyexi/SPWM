#include "pid.h"
#include "math.h"


pid_typedef pid;

void PID_Init(void)
{
	pid.Set_Value=24.0;
	pid.Actual_Value=0.0;
	pid.err=0.0;
	pid.intergral=0.0;
	pid.last_err=0.0;
	pid.Kp=0.0063;				//0.0063
	pid.Ki=0.0019;		 		//0.0020
	pid.Kd=0.002;					//0.007
	pid.out=0.0;
	pid.Out_MAX=1.0;
	pid.Out_MIN=0.5;
	pid.i_band=100.0;//100.0

}

float PID_realize(float ActualValue)
{
	
		pid.Actual_Value = ActualValue;
		pid.err = pid.Set_Value - pid.Actual_Value;
//		pid.intergral += pid.err;
	
		/*如果控制量大于最大值，则累积负误差*/
		if(pid.out>=pid.Out_MAX)
		{
			if(pid.err<=0)
			{
				pid.intergral+=pid.err;
			}
		}
		/*如果控制量小于最小值，则累积正误差*/
		else if(pid.out<=pid.Out_MIN)
		{
			if(pid.err>=0)
			{
				pid.intergral+=pid.err;
			}
		}
		/*积分分离和积分限幅*/
		else
		{
			if(fabs(pid.err)<=pid.i_band)//如果err小于i限制值则累积所有误差
			{
				pid.intergral+=pid.err;
			}
			else
			{
				pid.intergral=0;//如果err大于i限制值，则积分项为0
			}
		}
	
	
	
		pid.out=pid.Kp*pid.err+pid.Ki*pid.intergral+pid.Kd*(pid.err-pid.last_err) ;
		pid.last_err = pid.err;
	
	
	/*pid out限制最大值最小值*/
		if(pid.out >=pid.Out_MAX)
				pid.out=pid.Out_MAX;
		
		if(pid.out<=pid.Out_MIN)
				pid.out=pid.Out_MIN;

	return pid.out;
	
}



