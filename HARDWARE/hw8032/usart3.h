#ifndef __usart3_H
#define __usart3_H

#include "stdio.h" 

#define USART3_RXBUFF_SIZE   256               //���崮��1 ���ջ�������С 256�ֽ�
	  	
extern float DisplayTable[6];
extern unsigned char  Usart3_RxCounter;  
extern unsigned char Usart3_RxBuff[USART3_RXBUFF_SIZE]; //�ⲿ�����������ļ����Ե��øñ���

void Usart3_Init(unsigned int bound);

#endif
