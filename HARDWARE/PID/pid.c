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
	
		/*����������������ֵ�����ۻ������*/
		if(pid.out>=pid.Out_MAX)
		{
			if(pid.err<=0)
			{
				pid.intergral+=pid.err;
			}
		}
		/*���������С����Сֵ�����ۻ������*/
		else if(pid.out<=pid.Out_MIN)
		{
			if(pid.err>=0)
			{
				pid.intergral+=pid.err;
			}
		}
		/*���ַ���ͻ����޷�*/
		else
		{
			if(fabs(pid.err)<=pid.i_band)//���errС��i����ֵ���ۻ��������
			{
				pid.intergral+=pid.err;
			}
			else
			{
				pid.intergral=0;//���err����i����ֵ���������Ϊ0
			}
		}
	
	
	
		pid.out=pid.Kp*pid.err+pid.Ki*pid.intergral+pid.Kd*(pid.err-pid.last_err) ;
		pid.last_err = pid.err;
	
	
	/*pid out�������ֵ��Сֵ*/
		if(pid.out >=pid.Out_MAX)
				pid.out=pid.Out_MAX;
		
		if(pid.out<=pid.Out_MIN)
				pid.out=pid.Out_MIN;

	return pid.out;
	
}



