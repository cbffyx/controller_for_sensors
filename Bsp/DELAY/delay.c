#include "delay.h"

static uint16_t  fac_us=0;//usʹ��
static uint16_t fac_ms=0;//msʹ��
static __IO uint32_t TimingDelay;

/*******************************************************************************
* @Function Name	: ��ʱ��ʼ��
* @Description	:ϵͳƵ��Ϊ8MHz .
* @Input		: None
* @Output	: None
* @Return	: None
*******************************************************************************/
void delay_init()	 
{
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);	//ѡ���ⲿʱ��  HCLK
	//fac_us=SystemCoreClock/8000000;	//Ϊϵͳʱ�ӵ�1/8  4  1000000
	//if(result==8)
	fac_us=1;	//Ϊϵͳʱ�ӵ�1/8  4  1000000
	//else if(result==4) fac_us=;
	fac_ms=(uint16_t)fac_us*1000;

/*
  SysTick->CTRL&=0xfffffffb;//bit2���,ѡ���ⲿʱ��  HCLK/8
	fac_us=1;		    
	fac_ms=(uint16_t)fac_us*1000;
*/
}								    

void delay_ms(uint16_t nms)
{	 		  	  
	  uint32_t ui_tmp = 0x00;  
    SysTick->LOAD = nms * fac_ms;  
    SysTick->VAL = 0x00;  
    SysTick->CTRL = 0x01;  
      
    do  
    {  
        ui_tmp = SysTick->CTRL;  
    }while((ui_tmp & 0x01) && (!(ui_tmp & (1 << 16))));  
      
    SysTick->CTRL = 0x00;  
    SysTick->VAL = 0x00;      
} 


void delay_us(uint16_t nus)
{	 		  	  
	 uint32_t ui_tmp = 0x00;  
    SysTick->LOAD = nus * fac_us;  
    SysTick->VAL = 0x00;  
    SysTick->CTRL = 0x01;  
      
    do  
    {  
        ui_tmp = SysTick->CTRL;  
    }while((ui_tmp & 0x01) && (!(ui_tmp & (1 << 16))));  
      
    SysTick->CTRL = 0x00;  
    SysTick->VAL = 0x00;      
}

void delay_min(uint8_t nmin)
{
	uint8_t i,j;
	for(i=0;i<nmin;i++)
	{
				for(j=0;j<60;j++)
			{
					delay_ms(1000);
			}
	}
}

	void delay_second(uint8_t ns)
{
	  uint8_t j;
		for(j=0;j<ns;j++)
		{
					delay_ms(1000);
		}
}
