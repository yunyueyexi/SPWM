#ifndef  __SPWM_H
#define  __SPWM_H
#include "sys.h"

//��ʱ��1����ͨ������					1���򿪻���     0���ر�
#define tim1_supply_switch	1
//��ʱ��1��������						1��������		0���ر�����
#define tim1_dead_switch	1

//��ʱ��8����ͨ������					1���򿪻���     0���ر�
#define tim8_supply_switch	1
//��ʱ��8��������						1��������		0���ر�����
#define tim8_dead_switch	1



#define spot   400          //һ��SPWM�����ڹ�������ĸ���
#define sin_median	2000		//��ֵ


extern u8 pfreq;

void spwm_init(u16 pfreq,u16 pse);
void tim8_init(u16 pfreq);


#endif

