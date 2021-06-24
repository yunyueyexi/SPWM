#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "spwm.h"




int main(void)
{	
	 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	delay_init();
	
	spwm_init(50,0);
	tim8_init(50);
	
	while(1)
	{
		
	};
}

