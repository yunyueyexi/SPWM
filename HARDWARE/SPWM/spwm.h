#ifndef  __SPWM_H
#define  __SPWM_H
#include "sys.h"

//定时器1互补通道开关					1：打开互补     0：关闭
#define tim1_supply_switch	1
//定时器1死区开关						1：打开死区		0：关闭死区
#define tim1_dead_switch	1

//定时器8互补通道开关					1：打开互补     0：关闭
#define tim8_supply_switch	1
//定时器8死区开关						1：打开死区		0：关闭死区
#define tim8_dead_switch	1



#define spot   400          //一个SPWM周期内规则采样的个数
#define sin_median	2000		//中值


extern u8 pfreq;

void spwm_init(u16 pfreq,u16 pse);
void tim8_init(u16 pfreq);


#endif

