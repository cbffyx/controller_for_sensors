#include "TIM.h"
	
volatile uint8_t LoRaNode_UART_receive_flag;
volatile uint8_t BlueTooth_UART_receive_flag;
volatile uint8_t Debug_UART_receive_flag;
volatile uint32_t SysTickSCount=0;
volatile uint32_t SysTickCount=0;
volatile uint8_t SysTickSCount_flag=0;
/**
  * 简介：该函数用于MCU对串口操作的帧结尾判断定时器初始化。              
  * 参数： arr单位为ms  
  * 返回值：无
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
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);//开启中断
	TIM_Cmd(TIM3,DISABLE);//开启外设

	//TIM3 NVIC 配置
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
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
	
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);//开启中断
	TIM_Cmd(TIM4,DISABLE);//开启外设

	//TIM3 NVIC 配置
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
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
	
	TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE);//开启中断
	TIM_Cmd(TIM6,DISABLE);//开启外设

	//TIM3 NVIC 配置
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	Debug_UART_receive_flag=0;
}

/**
  * 简介：该函数用于MCU对LoRaWAN节点模块的串口操作的帧结尾判断定时器回调函数。              
  * 参数： LoRaNode_UART_receive_flag为串口接收到帧标志  
  * 返回值：无
  */
void TIM3_IRQHandler(void)//LoraUse
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)!=RESET)
	{
		//必须清楚标志位
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
    LoRaNode_UART_receive_flag = 1;
		TIM_Cmd(TIM3, DISABLE);//关闭中断
	}
}


/**
  * 简介：该函数用于MCU对蓝牙模块的串口操作的帧结尾判断定时器回调函数。              
  * 参数： BlueTooth_UART_receive_flag为串口接收到帧标志  
  * 返回值：无
  */
/*
void TIM4_IRQHandler(void)//BlueToothUse
{
	if(TIM_GetITStatus(TIM4,TIM_IT_Update)!=RESET)
	{
		//必须清楚标志位
		TIM_ClearITPendingBit(TIM4,TIM_IT_Update);
    BlueTooth_UART_receive_flag = 1;
		TIM_Cmd(TIM4, DISABLE);//关闭中断
	}
}
*/

/**
  * 简介：该函数用于MCU对调试串口操作的帧结尾判断定时器回调函数。              
  * 参数： Debug_UART_receive_flag为串口接收到帧标志  
  * 返回值：无
  */
void TIM6_IRQHandler(void)//DebugUse
{
	if(TIM_GetITStatus(TIM6,TIM_IT_Update)!=RESET)
	{
		//必须清楚标志位
		TIM_ClearITPendingBit(TIM6,TIM_IT_Update);
    Debug_UART_receive_flag = 1;
		TIM_Cmd(TIM6, DISABLE);//关闭中断
	}
}

//-----------------------------TIM7-----------------------------//
//该部分函数主要为应用程序提供超时判断，包含
//定时器初始化，开始，停止以及中断回调函数。
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
	
	TIM_ITConfig(TIM7,TIM_IT_Update,ENABLE);//开启中断
	TIM_Cmd(TIM7,DISABLE);//开启外设

	//TIM3 NVIC 配置
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	
}

//-----------------------------TIM2-----------------------------//
//该部分函数主要为应用程序提供超时判断，包含
//定时器初始化，开始，停止以及中断回调函数。
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
	
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);//开启中断

	//TIM3 NVIC 配置
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	
	TIM_Cmd(TIM2,ENABLE);//开启外设
}

void TIM7_IRQHandler(void)//BaseUse 秒计时
{
	if(TIM_GetITStatus(TIM7,TIM_IT_Update)!=RESET)
	{
		//必须清除标志位
		TIM_ClearITPendingBit(TIM7,TIM_IT_Update);
		SysTickSCount--;
		if (SysTickSCount==0)
		{
			SysTickSCount_flag=1;
			TIM_Cmd(TIM7, DISABLE);//关闭中断
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
		//必须清楚标志位
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
