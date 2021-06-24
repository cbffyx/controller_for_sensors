#include "TIM.h"
	
volatile uint8_t LoRaNode_UART_receive_flag;
volatile uint8_t BlueTooth_UART_receive_flag;
volatile uint8_t Debug_UART_receive_flag;
volatile uint32_t SysTickSCount=0;
volatile uint32_t SysTickCount=0;
volatile uint8_t SysTickSCount_flag=0;
/**
  * ��飺�ú�������MCU�Դ��ڲ�����֡��β�ж϶�ʱ����ʼ����              
  * ������ arr��λΪms  
  * ����ֵ����
  */
void TIM3_UART_Init_ms(uint16_t arr)//LoraUse
{   
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	TIM_DeInit(TIM3);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
	
	TIM_TimeBaseStructure.TIM_Period=arr-1;
	TIM_TimeBaseStructure.TIM_Prescaler=7999;
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);//�����ж�
	TIM_Cmd(TIM3,DISABLE);//��������

	//TIM3 NVIC ����
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	LoRaNode_UART_receive_flag=0;
}

/*
void TIM4_UART_Init_ms(uint16_t arr)//BlueToothUse
{   
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	TIM_DeInit(TIM4);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	TIM_ClearITPendingBit(TIM4,TIM_IT_Update);
	
	TIM_TimeBaseStructure.TIM_Period=arr-1;
	TIM_TimeBaseStructure.TIM_Prescaler=7999;
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseStructure);
	
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);//�����ж�
	TIM_Cmd(TIM4,DISABLE);//��������

	//TIM3 NVIC ����
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	BlueTooth_UART_receive_flag=0;
}
*/

void TIM6_UART_Init_ms(uint16_t arr)//DebugUse
{   
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	TIM_DeInit(TIM6);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
	TIM_ClearITPendingBit(TIM6,TIM_IT_Update);
	
	TIM_TimeBaseStructure.TIM_Period=arr-1;
	TIM_TimeBaseStructure.TIM_Prescaler=7999;
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM6,&TIM_TimeBaseStructure);
	
	TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE);//�����ж�
	TIM_Cmd(TIM6,DISABLE);//��������

	//TIM3 NVIC ����
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	Debug_UART_receive_flag=0;
}

/**
  * ��飺�ú�������MCU��LoRaWAN�ڵ�ģ��Ĵ��ڲ�����֡��β�ж϶�ʱ���ص�������              
  * ������ LoRaNode_UART_receive_flagΪ���ڽ��յ�֡��־  
  * ����ֵ����
  */
void TIM3_IRQHandler(void)//LoraUse
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)!=RESET)
	{
		//���������־λ
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
    LoRaNode_UART_receive_flag = 1;
		TIM_Cmd(TIM3, DISABLE);//�ر��ж�
	}
}


/**
  * ��飺�ú�������MCU������ģ��Ĵ��ڲ�����֡��β�ж϶�ʱ���ص�������              
  * ������ BlueTooth_UART_receive_flagΪ���ڽ��յ�֡��־  
  * ����ֵ����
  */
/*
void TIM4_IRQHandler(void)//BlueToothUse
{
	if(TIM_GetITStatus(TIM4,TIM_IT_Update)!=RESET)
	{
		//���������־λ
		TIM_ClearITPendingBit(TIM4,TIM_IT_Update);
    BlueTooth_UART_receive_flag = 1;
		TIM_Cmd(TIM4, DISABLE);//�ر��ж�
	}
}
*/

/**
  * ��飺�ú�������MCU�Ե��Դ��ڲ�����֡��β�ж϶�ʱ���ص�������              
  * ������ Debug_UART_receive_flagΪ���ڽ��յ�֡��־  
  * ����ֵ����
  */
void TIM6_IRQHandler(void)//DebugUse
{
	if(TIM_GetITStatus(TIM6,TIM_IT_Update)!=RESET)
	{
		//���������־λ
		TIM_ClearITPendingBit(TIM6,TIM_IT_Update);
    Debug_UART_receive_flag = 1;
		TIM_Cmd(TIM6, DISABLE);//�ر��ж�
	}
}

//-----------------------------TIM7-----------------------------//
//�ò��ֺ�����ҪΪӦ�ó����ṩ��ʱ�жϣ�����
//��ʱ����ʼ������ʼ��ֹͣ�Լ��жϻص�������
//--------------------------------------------------------------//
void TIM7_Init_s(void)//BaseUse
{   
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	TIM_DeInit(TIM7);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);
	TIM_ClearITPendingBit(TIM7,TIM_IT_Update);
	
	TIM_TimeBaseStructure.TIM_Period=999;
	TIM_TimeBaseStructure.TIM_Prescaler=7999;
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM7,&TIM_TimeBaseStructure);
	
	TIM_ITConfig(TIM7,TIM_IT_Update,ENABLE);//�����ж�
	TIM_Cmd(TIM7,DISABLE);//��������

	//TIM3 NVIC ����
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	
}

//-----------------------------TIM2-----------------------------//
//�ò��ֺ�����ҪΪӦ�ó����ṩ��ʱ�жϣ�����
//��ʱ����ʼ������ʼ��ֹͣ�Լ��жϻص�������
//--------------------------------------------------------------//
void TIM2_Init_s(void)//BaseUse
{   
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	TIM_DeInit(TIM2);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
	
	TIM_TimeBaseStructure.TIM_Period=1;
	TIM_TimeBaseStructure.TIM_Prescaler=3999;
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);
	
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);//�����ж�

	//TIM3 NVIC ����
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	
	TIM_Cmd(TIM2,ENABLE);//��������
}

void TIM7_IRQHandler(void)//BaseUse ���ʱ
{
	if(TIM_GetITStatus(TIM7,TIM_IT_Update)!=RESET)
	{
		//���������־λ
		TIM_ClearITPendingBit(TIM7,TIM_IT_Update);
		SysTickSCount--;
		if (SysTickSCount==0)
		{
			SysTickSCount_flag=1;
			TIM_Cmd(TIM7, DISABLE);//�ر��ж�
		}
		else
		{
		}
	}
}

void TIM2_IRQHandler(void)//BaseUse ms
{
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)!=RESET)
	{
		//���������־λ
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
		SysTickCount++;
	}
}

uint32_t HAL_GetTickS(void)
{
	return SysTickSCount;
}

uint32_t HAL_GetTick(void)
{
	return SysTickCount;
}
