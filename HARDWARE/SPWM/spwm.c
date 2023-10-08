#include "spwm.h"
#include "usart3.h"
#include "pid.h"

extern float Vdata;
u8 pfreq = 50;           //默认频率50Hz
__IO float Period_percent;

float k = 0.85;

u32 spwm[spot] ={
2000,2031,2062,2094,2125,2156,2188,2219,2250,2281,2312,2343,2374,2405,2436,2466,2497,2527,2557,2588,
2618,2647,2677,2706,2736,2765,2794,2823,2851,2879,2907,2935,2963,2990,3018,3044,3071,3098,3124,3150,
3175,3200,3225,3250,3274,3298,3322,3346,3369,3391,3414,3436,3457,3479,3500,3520,3541,3560,3580,3599,
3618,3636,3654,3671,3688,3705,3721,3737,3752,3767,3782,3796,3809,3822,3835,3847,3859,3870,3881,3892,
3902,3911,3920,3929,3937,3944,3951,3958,3964,3970,3975,3980,3984,3987,3991,3993,3996,3997,3999,3999,
4000,3999,3999,3997,3996,3993,3991,3987,3984,3980,3975,3970,3964,3958,3951,3944,3937,3929,3920,3911,
3902,3892,3881,3870,3859,3847,3835,3822,3809,3796,3782,3767,3752,3737,3721,3705,3688,3671,3654,3636,
3618,3599,3580,3560,3541,3520,3500,3479,3457,3436,3414,3391,3369,3346,3322,3298,3274,3250,3225,3200,
3175,3150,3124,3098,3071,3044,3018,2990,2963,2935,2907,2879,2851,2823,2794,2765,2736,2706,2677,2647,
2618,2588,2557,2527,2497,2466,2436,2405,2374,2343,2312,2281,2250,2219,2188,2156,2125,2094,2062,2031,
2000,1968,1937,1905,1874,1843,1811,1780,1749,1718,1687,1656,1625,1594,1563,1533,1502,1472,1442,1411,
1381,1352,1322,1293,1263,1234,1205,1176,1148,1120,1092,1064,1036,1009,981,955,928,901,875,849,
824,799,774,749,725,701,677,653,630,608,585,563,542,520,499,479,458,439,419,400,
381,363,345,328,311,294,278,262,247,232,217,203,190,177,164,152,140,129,118,107,
97,88,79,70,62,55,48,41,35,29,24,19,15,12,8,6,3,2,0,0,
0,0,0,2,3,6,8,12,15,19,24,29,35,41,48,55,62,70,79,88,
97,107,118,129,140,152,164,177,190,203,217,232,247,262,278,294,311,328,345,363,
381,400,419,439,458,479,499,520,542,563,585,608,630,653,677,701,725,749,774,799,
824,849,875,901,928,955,981,1009,1036,1064,1092,1120,1148,1176,1205,1234,1263,1293,1322,1352,
1381,1411,1442,1472,1502,1533,1563,1594,1625,1656,1687,1718,1749,1780,1811,1843,1874,1905,1937,1968

};


static u16 i = 0;       //取样点
u16 TimerPeriod = 0;   //重装载值
u16 Channel3Pulse = 0;



void spwm_init(u16 pfreq,u16 pse)
{
	
	GPIO_InitTypeDef GPIO_InitStruct;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_OCInitTypeDef TIM_OCInitStruct;
	TIM_BDTRInitTypeDef TIM_BDTRInitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;

	TimerPeriod = (SystemCoreClock / (spot*pfreq) ) - 1;
	Channel3Pulse = (u16)((u32)(50  * (TimerPeriod - 1)) / 100 );	//占空比50%
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
#if tim1_supply_switch
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
#endif
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);
	
#if tim1_supply_switch
	GPIO_PinRemapConfig(GPIO_PartialRemap_TIM1,ENABLE);
#endif	
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;                       //推挽输出模式
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
#if tim1_supply_switch	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
#endif

	TIM_TimeBaseInitStruct.TIM_ClockDivision = 0;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = TimerPeriod;
	TIM_TimeBaseInitStruct.TIM_Prescaler = pse;
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM1,&TIM_TimeBaseInitStruct);
	
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStruct.TIM_OCIdleState = TIM_OCIdleState_Reset;
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
	
#if tim1_supply_switch
	TIM_OCInitStruct.TIM_OCNPolarity = TIM_OCNPolarity_High;
	TIM_OCInitStruct.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
	TIM_OCInitStruct.TIM_OutputNState = TIM_OutputNState_Enable;
#endif	
	
	TIM_OCInitStruct.TIM_Pulse = Channel3Pulse;
	TIM_OC3Init(TIM1,&TIM_OCInitStruct);
	
#if tim1_dead_switch
	TIM_BDTRInitStruct.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;
	TIM_BDTRInitStruct.TIM_Break = TIM_Break_Disable;
	TIM_BDTRInitStruct.TIM_BreakPolarity = TIM_BreakPolarity_High;
	TIM_BDTRInitStruct.TIM_DeadTime = 0x54;                        
	TIM_BDTRInitStruct.TIM_LOCKLevel = TIM_LOCKLevel_OFF;
	TIM_BDTRInitStruct.TIM_OSSIState = TIM_OSSIState_Disable;
	TIM_BDTRInitStruct.TIM_OSSRState = TIM_OSSRState_Disable;
	TIM_BDTRConfig(TIM1,&TIM_BDTRInitStruct);
#endif	
	
	NVIC_InitStruct.NVIC_IRQChannel = TIM1_UP_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStruct);
	
	TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE);
	TIM_OC3PreloadConfig(TIM1,TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM1,ENABLE);
	TIM_Cmd(TIM1,ENABLE);
	TIM_CtrlPWMOutputs(TIM1,ENABLE);
	
}



void tim8_init(u16 pfreq)
{
	
	GPIO_InitTypeDef GPIO_InitStruct;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_OCInitTypeDef TIM_OCInitStruct;
	TIM_BDTRInitTypeDef TIM_BDTRInitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	
#if tim8_supply_switch
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
#endif
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8,ENABLE);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;                       //推挽输出模式
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_InitStruct);
	
#if tim8_supply_switch
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
#endif

	TIM_TimeBaseInitStruct.TIM_ClockDivision = 0;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = pfreq-1;
	TIM_TimeBaseInitStruct.TIM_Prescaler = 28800-1;
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM8,&TIM_TimeBaseInitStruct);
	
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStruct.TIM_OCIdleState = TIM_OCIdleState_Reset;
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
	
#if tim8_supply_switch
	TIM_OCInitStruct.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
	TIM_OCInitStruct.TIM_OCNPolarity = TIM_OCNPolarity_High;
	TIM_OCInitStruct.TIM_OutputNState = TIM_OutputNState_Enable;
#endif	
	
	TIM_OCInitStruct.TIM_Pulse = pfreq/2;
	TIM_OC1Init(TIM8,&TIM_OCInitStruct);

#if tim8_dead_switch
	TIM_BDTRInitStruct.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;
	TIM_BDTRInitStruct.TIM_Break = TIM_Break_Disable;
	TIM_BDTRInitStruct.TIM_BreakPolarity = TIM_BreakPolarity_High;
	TIM_BDTRInitStruct.TIM_DeadTime = 0x54;                        
	TIM_BDTRInitStruct.TIM_LOCKLevel = TIM_LOCKLevel_OFF;
	TIM_BDTRInitStruct.TIM_OSSIState = TIM_OSSIState_Disable;
	TIM_BDTRInitStruct.TIM_OSSRState = TIM_OSSRState_Disable;
	TIM_BDTRConfig(TIM8,&TIM_BDTRInitStruct);
#endif

	TIM_OC1PreloadConfig(TIM8,TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM8,ENABLE);
	TIM_Cmd(TIM8,ENABLE);
	TIM_CtrlPWMOutputs(TIM8,ENABLE);
	
}





void TIM1_UP_IRQHandler(void)
{
	
	u16 new_spwm;
	float temp;
	static u8 j=0;

	float Period_percent;

	temp = (SystemCoreClock / (spot*pfreq) ) - 1;
	Period_percent = temp / 4000.0; 
	
	if(TIM_GetITStatus(TIM1,TIM_IT_Update)!=RESET)
	{
		
		TIM_SetAutoreload(TIM1,temp);
		if(spwm[i]>sin_median)
		{
			new_spwm = k * (spwm[i] - sin_median) + sin_median;
			TIM_SetCompare3(TIM1,(u16)(Period_percent * new_spwm));
		}
		else if(spwm[i]<sin_median)
		{
			new_spwm = sin_median - k * (sin_median - spwm[i]);
			TIM_SetCompare3(TIM1,(u16)(Period_percent * new_spwm));
		}
		else
		{
			TIM_SetCompare3(TIM1,(u16)(Period_percent*spwm[i]));
		}
		i++;
		if(i==spot)
		{	
			i=0;
			j++;
			if(j==7)
			{
				k=PID_realize(Vdata);
				j=0;
			}
				
		}
		TIM_ClearITPendingBit(TIM1,TIM_IT_Update);
	}
	
}
