
#include "stm32f10x.h"  //包含需要的头文件
#include "usart3.h"	
#include "string.h"
#include "24c02.h"      //包含需要的头文件

unsigned char Usart3_RxCounter;          //外部声明，其他文件可以调用该变量
unsigned char Usart3_RxFlag=0;          //外部声明，其他文件可以调用该变量
unsigned char Usart3_RxBuff[USART3_RXBUFF_SIZE]; //外部声明，其他文件可以调用该变量
unsigned char Usart3_Receive_Flag=0;
unsigned char HLW8032_SumCheck=0;

unsigned long Voltage_Parameter_REG=0; //电压参数寄存器
unsigned long Voltage_REG=0;//电压寄存器
unsigned long Current_Parameter_REG=0; //电压参数寄存器
unsigned long Current_REG=0;//电压寄存器
unsigned long Power_Parameter_REG=0; //电压参数寄存器
unsigned long Power_REG=0;//电压寄存器
unsigned int Energy_count=0;
unsigned char DATA_REG_BIT7=0;  //数据更新寄存器BIT7

float DisplayTable[6];

void Usart3_Init(unsigned int bound)
{ 
   //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	  //打开时钟
 
	/*  配置GPIO的模式和IO口 */
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;//TX			     //串口输出PB10
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;	         //复用推挽输出
	GPIO_Init(GPIOB,&GPIO_InitStructure);                  /* 初始化串口输入IO */
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_11;                //RX			 //串口输入PB11
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;		  //浮空输入
	GPIO_Init(GPIOB,&GPIO_InitStructure);                   /* 初始化GPIO */
	
   //USART3 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
  USART_Init(USART3, &USART_InitStructure); //初始化串口3
	
  USART_Cmd(USART3, ENABLE);  //使能串口3 
	
	USART_ClearFlag(USART3, USART_FLAG_TC);
	
	USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);//开启空闲中断
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启相关中断

	//Usart3 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;//串口3中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;//抢占优先级4
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//子优先级4
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	
}

/*******************************************************************************
* 函 数 名         : USART3_IRQHandler
* 函数功能		     : USART3中断函数
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/ 
void USART3_IRQHandler(void)
{
	unsigned long Energy_CNT=0;
	unsigned char Rec_Data;
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		Rec_Data =USART_ReceiveData(USART3);//(USART1->DR);	//读取接收到的数据
		switch(Usart3_Receive_Flag)
		{
case 0:
{
	if(Rec_Data==0x55||Rec_Data==0xf2)
	{
		Usart3_RxBuff[0]=Rec_Data;
		Usart3_Receive_Flag=1;
	}
	break;	
}
case 1:
{
	if(Rec_Data==0x5A)
	{
		Usart3_RxBuff[1]=Rec_Data;
		Usart3_RxCounter=2;
		Usart3_Receive_Flag=2;
		HLW8032_SumCheck=0;
	}
	else 
	{
		Usart3_Receive_Flag=0;
	}	
	break;				
}
			case 2:
			{
				Usart3_RxBuff[Usart3_RxCounter]=Rec_Data;
				Usart3_RxCounter++;
				if(Usart3_RxCounter==24)
				{
					Usart3_Receive_Flag=0;
					if(HLW8032_SumCheck==Rec_Data)  //校验
					{
						USART_Cmd(USART3, DISABLE);  //使能串口3 ,关闭读取电参数
						Voltage_Parameter_REG=Usart3_RxBuff[2]*65536+Usart3_RxBuff[3]*256+Usart3_RxBuff[4];
						Voltage_REG=Usart3_RxBuff[5]*65536+Usart3_RxBuff[6]*256+Usart3_RxBuff[7];
						Current_Parameter_REG=Usart3_RxBuff[8]*65536+Usart3_RxBuff[9]*256+Usart3_RxBuff[10];
						Current_REG=Usart3_RxBuff[11]*65536+Usart3_RxBuff[12]*256+Usart3_RxBuff[13];
						Power_Parameter_REG=Usart3_RxBuff[14]*65536+Usart3_RxBuff[15]*256+Usart3_RxBuff[16];
						Power_REG=Usart3_RxBuff[17]*65536+Usart3_RxBuff[18]*256+Usart3_RxBuff[19];	
						
						DisplayTable[0]=(float)Voltage_Parameter_REG*2.000/Voltage_REG; //电压
						//if(DisplayTable[0]<2)  DisplayTable[0]=0;  //监测到电压小于2，认为是假值，归0
//						if((Usart3_RxBuff[0]==0xf2)&&(((Usart3_RxBuff[20]>>4)&1)==0)) //功率溢出且功率未更新，认为功率为0，不能使用公式计算
//						{
//							DisplayTable[2]=0;
//							DisplayTable[3]=0;
//						}
//						else
						{
							DisplayTable[1]=(float)Current_Parameter_REG/(Current_REG*0.05);  //电流
							DisplayTable[2]=(float)Power_Parameter_REG*2.000/Power_REG;//功率
							DisplayTable[3]=DisplayTable[2]/(DisplayTable[0]*DisplayTable[1]);//功率因数
						}
						if(DATA_REG_BIT7!=((Usart3_RxBuff[20]>>7)&1))
						{
							Energy_count++;
							DATA_REG_BIT7=(Usart3_RxBuff[20]>>7)&1;
						}
						Energy_CNT=Energy_count*65536+Usart3_RxBuff[21]*256+Usart3_RxBuff[22];
						DisplayTable[4]=(float)Energy_CNT/1000000000*Power_Parameter_REG*3.006*1/3600;//电能
					}
				}
				else	HLW8032_SumCheck+=Rec_Data;
				break;
			}
			default: break;	
		}
  } 
}
