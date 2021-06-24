#include "spwm.h"

u8 pfreq = 50;           //默认频率50Hz
__IO float Period_percent;

float k = 1;

const u32 spwm[spot] ={
0,63,126,188,251,314,376,439,501,564,626,688,750,811,873,934,995,1055,1116,1176,
1236,1296,1355,1414,1472,1531,1589,1646,1703,1760,1816,1872,1927,1982,2036,2090,2143,2196,2248,2300,
2351,2402,2452,2501,2550,2598,2645,2692,2738,2784,2828,2873,2916,2959,3000,3042,3082,3122,3161,3199,
3236,3273,3308,3343,3377,3411,3443,3475,3505,3535,3564,3592,3619,3646,3671,3696,3719,3742,3764,3784,
3804,3823,3841,3858,3874,3889,3904,3917,3929,3940,3951,3960,3968,3976,3982,3988,3992,3996,3998,4000,
4000,4000,3998,3996,3992,3988,3982,3976,3968,3960,3951,3940,3929,3917,3904,3889,3874,3858,3841,3823,
3804,3784,3764,3742,3719,3696,3671,3646,3619,3592,3564,3535,3505,3475,3443,3411,3377,3343,3308,3273,
3236,3199,3161,3122,3082,3042,3000,2959,2916,2873,2828,2784,2738,2692,2645,2598,2550,2501,2452,2402,
2351,2300,2248,2196,2143,2090,2036,1982,1927,1872,1816,1760,1703,1646,1589,1531,1472,1414,1355,1296,
1236,1176,1116,1055,995,934,873,811,750,688,626,564,501,439,376,314,251,188,126,63,
	
	
4000,3937,3874,3812,3749,3686,3624,3561,3499,3436,3374,3312,3250,3189,3127,3066,3005,2945,2884,2824,
2764,2704,2645,2586,2528,2469,2411,2354,2297,2240,2184,2128,2073,2018,1964,1910,1857,1804,1752,1700,
1649,1598,1548,1499,1450,1402,1355,1308,1262,1216,1172,1127,1084,1041,1000,958,918,878,839,801,
764,727,692,657,623,589,557,525,495,465,436,408,381,354,329,304,281,258,236,216,
196,177,159,142,126,111,96,83,71,60,49,40,32,24,18,12,8,4,2,0,
0,0,2,4,8,12,18,24,32,40,49,60,71,83,96,111,126,142,159,177,
196,216,236,258,281,304,329,354,381,408,436,465,495,525,557,589,623,657,692,727,
764,801,839,878,918,958,1000,1041,1084,1127,1172,1216,1262,1308,1355,1402,1450,1499,1548,1598,
1649,1700,1752,1804,1857,1910,1964,2018,2073,2128,2184,2240,2297,2354,2411,2469,2528,2586,2645,2704,
2764,2824,2884,2945,3005,3066,3127,3189,3250,3312,3374,3436,3499,3561,3624,3686,3749,3812,3874,3937

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
		if(i==spot)i=0;
		TIM_ClearITPendingBit(TIM1,TIM_IT_Update);
	}
	
}


