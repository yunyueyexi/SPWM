#include "delay.h"
#include "sys.h"
#include "spwm.h"
#include "OLED.h"
#include "usart3.h"
#include "key.h"
#include "adc.h"

//extern float k;
float Vdata;
float data[10],temp;
#define ARRAY_SIZE 400



extern u32 spwm[ARRAY_SIZE]; 

void shiftArray(u32 spwm[], int shift,int adcx);  //相位调整函数
//float Adjust(float data[10],int i);      //幅度调整函数

int main(void)
{	
//	int i = 0;
//	float num = 0.0;
	const int shift = 2;
	int adcx = 0;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	delay_init();
	
	
	Usart3_Init(4800);              //串口3功能初始化，波特率4800
	KEY_Init();
	spwm_init(50,0);
	tim8_init(50);
	Adc_Init();		  		//ADC初始化
	OLED_Init();
	
	
	while(1)
	{
			adcx = Get_Adc_Average(ADC_Channel_1,200); //读取电压峰峰值
			shiftArray(spwm,shift,adcx);
			USART_Cmd(USART3, ENABLE);  //使能串口3 ,读取电参数
			delay_ms(200);
			Vdata = DisplayTable[0];
//			data[i] = DisplayTable[0];
//		  num = Adjust(data,i);
//			i++;
		
	
			OLED_ShowCN(1,5,3);OLED_ShowCN(1,7,4);OLED_ShowCN(1,9,5);OLED_ShowCN(1,11,6);
			OLED_ShowCN(2,1,0);OLED_ShowCN(2,3,1);OLED_ShowString(2,5,"U/V");OLED_ShowNum(2,9,Vdata,2,2); //电压显示
			OLED_ShowCN(3,1,0);OLED_ShowCN(3,3,1);OLED_ShowString(3,5,"U/V");OLED_ShowNum(3,9,spwm[0],4,0); //电压显示
			OLED_ShowCN(4,1,0);OLED_ShowCN(4,3,1);OLED_ShowString(4,5,"U/V");OLED_ShowNum(4,9,adcx,3,0); //电压显示

		  //delay_ms(750);
	}
}


 //相位调整函数
void shiftArray(u32 spwm[], int shift,int adcx)
 {
	int i,shift1 = 2,shift2 = 1;
	u32 temp[8];
	if(adcx >= 99)
		{
    for ( i = ARRAY_SIZE - shift; i < ARRAY_SIZE; i++) 
			{
        temp[i - (ARRAY_SIZE - shift)] = spwm[i];
    }

    for ( i = ARRAY_SIZE - 1; i >= shift; i--) 
		{
        spwm[i] = spwm[i - shift];
    }

    for ( i = 0; i < shift; i++) 
		{
        spwm[i] = temp[i];
    }
		for(i = 0;i < shift;i++)
		{
			temp[i] = 0;
		}
		}
		
		else if(adcx < 99 && adcx >50 )
		{
			    for ( i = ARRAY_SIZE - shift1; i < ARRAY_SIZE; i++) 
					{
						temp[i - (ARRAY_SIZE - shift1)] = spwm[i];
					}

					for ( i = ARRAY_SIZE - 1; i >= shift; i--) 
					{
						spwm[i] = spwm[i - shift1];
					}

					for ( i = 0; i < shift1; i++) 
					{
						spwm[i] = temp[i];
					}
					for(i = 0;i < shift1;i++)
					{
						temp[i] = 0;
					}
			}
					
	  else if(adcx < 50 && adcx >= 25 )
  	{ 
			    for ( i = ARRAY_SIZE - shift2; i < ARRAY_SIZE; i++) 
					{
						temp[i - (ARRAY_SIZE - shift2)] = spwm[i];
					}

					for ( i = ARRAY_SIZE - 1; i >= shift2; i--) 
					{
						spwm[i] = spwm[i - shift2];
					}

					for ( i = 0; i < shift2; i++) 
					{
						spwm[i] = temp[i];
					}
					for(i = 0;i < shift2;i++)
					{
						temp[i] = 0;
					}
		}
	}
//		}

//		else ;

//	else if()
//	{
//			  key = KEY_Scan(1);
//	  if(key == KEY1_PRES)
//		{
//			temp[0] = spwm[ARRAY_SIZE - 1];
//			temp[1] = spwm[ARRAY_SIZE - 2];
//    
//			for ( i =ARRAY_SIZE - 3 ; i >= 0; i--)
//			{
//        spwm[i + 2] = spwm[i];
//			}
//    
//			spwm[0] = temp[1];
//			spwm[1] = temp[0]
//	}



 //幅度调整函数
//float Adjust(float data[10],int i)
//{
//	float num = 0.00;
//	int j = 0,n;
//			  if(i == 9)  
//			{    for (n = 1; n< 10; n++) 
//				{
//					 temp = data[n];
//					 j = n - 1;
//        
//					 while (j >= 0 && data[j] > temp)
//						 {
//								data[j + 1] = data[j];
//								j--;
//						 }
//        data[j + 1] = temp;
//				}
//				num = (data[1]+data[2]+data[3]+data[4]+data[5]+data[6]+data[7]+data[8])/8 -0.3;
//				if(num <= 23.40 + 0.1)                      k += 0.02;
//				else if(num >= 24.60 +0.1)                 k -= 0.02;
//				else if(num <= 23.60 +0.1 && num > 23.40 +0.1)            k += 0.005;
//				else if(num >= 24.40 +0.1 && num < 24.60 +0.1)            k -= 0.005;
//				else if(num <= 23.97 +0.1 && num > 23.60 +0.1)            k += 0.0001;
//				else if(num >= 24.03 +0.1 && num < 24.40 +0.1)            k -= 0.0001;
//				else k = k;				
//				i = 0;
//			}
//			delay_ms(100);
//			
//			return num;
//}



