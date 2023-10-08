
#include "stm32f10x.h"  //������Ҫ��ͷ�ļ�
#include "usart3.h"	
#include "string.h"
#include "24c02.h"      //������Ҫ��ͷ�ļ�

unsigned char Usart3_RxCounter;          //�ⲿ�����������ļ����Ե��øñ���
unsigned char Usart3_RxFlag=0;          //�ⲿ�����������ļ����Ե��øñ���
unsigned char Usart3_RxBuff[USART3_RXBUFF_SIZE]; //�ⲿ�����������ļ����Ե��øñ���
unsigned char Usart3_Receive_Flag=0;
unsigned char HLW8032_SumCheck=0;

unsigned long Voltage_Parameter_REG=0; //��ѹ�����Ĵ���
unsigned long Voltage_REG=0;//��ѹ�Ĵ���
unsigned long Current_Parameter_REG=0; //��ѹ�����Ĵ���
unsigned long Current_REG=0;//��ѹ�Ĵ���
unsigned long Power_Parameter_REG=0; //��ѹ�����Ĵ���
unsigned long Power_REG=0;//��ѹ�Ĵ���
unsigned int Energy_count=0;
unsigned char DATA_REG_BIT7=0;  //���ݸ��¼Ĵ���BIT7

float DisplayTable[6];

void Usart3_Init(unsigned int bound)
{ 
   //GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	  //��ʱ��
 
	/*  ����GPIO��ģʽ��IO�� */
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;//TX			     //�������PB10
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;	         //�����������
	GPIO_Init(GPIOB,&GPIO_InitStructure);                  /* ��ʼ����������IO */
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_11;                //RX			 //��������PB11
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;		  //��������
	GPIO_Init(GPIOB,&GPIO_InitStructure);                   /* ��ʼ��GPIO */
	
   //USART3 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  USART_Init(USART3, &USART_InitStructure); //��ʼ������3
	
  USART_Cmd(USART3, ENABLE);  //ʹ�ܴ���3 
	
	USART_ClearFlag(USART3, USART_FLAG_TC);
	
	USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);//���������ж�
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//��������ж�

	//Usart3 NVIC ����
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;//����3�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;//��ռ���ȼ�4
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//�����ȼ�4
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	
}

/*******************************************************************************
* �� �� ��         : USART3_IRQHandler
* ��������		     : USART3�жϺ���
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/ 
void USART3_IRQHandler(void)
{
	unsigned long Energy_CNT=0;
	unsigned char Rec_Data;
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		Rec_Data =USART_ReceiveData(USART3);//(USART1->DR);	//��ȡ���յ�������
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
					if(HLW8032_SumCheck==Rec_Data)  //У��
					{
						USART_Cmd(USART3, DISABLE);  //ʹ�ܴ���3 ,�رն�ȡ�����
						Voltage_Parameter_REG=Usart3_RxBuff[2]*65536+Usart3_RxBuff[3]*256+Usart3_RxBuff[4];
						Voltage_REG=Usart3_RxBuff[5]*65536+Usart3_RxBuff[6]*256+Usart3_RxBuff[7];
						Current_Parameter_REG=Usart3_RxBuff[8]*65536+Usart3_RxBuff[9]*256+Usart3_RxBuff[10];
						Current_REG=Usart3_RxBuff[11]*65536+Usart3_RxBuff[12]*256+Usart3_RxBuff[13];
						Power_Parameter_REG=Usart3_RxBuff[14]*65536+Usart3_RxBuff[15]*256+Usart3_RxBuff[16];
						Power_REG=Usart3_RxBuff[17]*65536+Usart3_RxBuff[18]*256+Usart3_RxBuff[19];	
						
						DisplayTable[0]=(float)Voltage_Parameter_REG*2.000/Voltage_REG; //��ѹ
						//if(DisplayTable[0]<2)  DisplayTable[0]=0;  //��⵽��ѹС��2����Ϊ�Ǽ�ֵ����0
//						if((Usart3_RxBuff[0]==0xf2)&&(((Usart3_RxBuff[20]>>4)&1)==0)) //��������ҹ���δ���£���Ϊ����Ϊ0������ʹ�ù�ʽ����
//						{
//							DisplayTable[2]=0;
//							DisplayTable[3]=0;
//						}
//						else
						{
							DisplayTable[1]=(float)Current_Parameter_REG/(Current_REG*0.05);  //����
							DisplayTable[2]=(float)Power_Parameter_REG*2.000/Power_REG;//����
							DisplayTable[3]=DisplayTable[2]/(DisplayTable[0]*DisplayTable[1]);//��������
						}
						if(DATA_REG_BIT7!=((Usart3_RxBuff[20]>>7)&1))
						{
							Energy_count++;
							DATA_REG_BIT7=(Usart3_RxBuff[20]>>7)&1;
						}
						Energy_CNT=Energy_count*65536+Usart3_RxBuff[21]*256+Usart3_RxBuff[22];
						DisplayTable[4]=(float)Energy_CNT/1000000000*Power_Parameter_REG*3.006*1/3600;//����
					}
				}
				else	HLW8032_SumCheck+=Rec_Data;
				break;
			}
			default: break;	
		}
  } 
}
