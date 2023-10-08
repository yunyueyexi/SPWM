#ifndef __usart3_H
#define __usart3_H

#include "stdio.h" 

#define USART3_RXBUFF_SIZE   256               //定义串口1 接收缓冲区大小 256字节
	  	
extern float DisplayTable[6];
extern unsigned char  Usart3_RxCounter;  
extern unsigned char Usart3_RxBuff[USART3_RXBUFF_SIZE]; //外部声明，其他文件可以调用该变量

void Usart3_Init(unsigned int bound);

#endif
