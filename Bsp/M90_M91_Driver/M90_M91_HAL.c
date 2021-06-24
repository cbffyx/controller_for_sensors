/**
  ******************************************************************************
  * 文件名 ：   M90_M91_HAL.c
  * 作者   ：   LSD RF/LoRaWAN Team
  * 版本   ：   V1.0.0
  * 时间   ：   19-May-2018
  * 文件描述：
  *     该文件为M90_M91模块的硬件层，包含MCU与M90模块的串口配置，GPIO口初始化，以
  *及用于各个超时判断的低功耗时钟等。
  *    客户在使用M90模块时候需要移植该文件，保证各个函数名、函数形参不变的情况下
  *根据自己的MCU平台修改函数内容，使各个功能块正常运行。硬件层占用资源如下：
  *
  *串口：本例程使用STM32L4的低功耗串口LPUART1与M90模块进行通信。
  *定时器：本例程使用了STM32L4的TIM3作为串口的帧结尾判断。
  *        使用了STM32L4的LPTIM1作为应用层需要的各个超时定时器。
  *        如果用户在其他地方需要用到这两个定时器，其接口都已留出，用户可在回调函
  *        数中自行添加
  *GPIO口：本例程使用的GPIO口详情如下：
  *        PC3  ---> WAKE 
  *        PC4  ---> STAT
  *        PC5  ---> BUSY     
  *        PA0  ---> MODE 
  *        PA1  ---> RST 
*******************************************************************************/

#include "stm32l1xx.h"
#include "M90_M91_HAL.h"

uint8_t LoRaNode_ReadTimeout_flag = 0,LoRaNode_BUSYTimeout_flag = 0,LoRaNode_LPsend_flag = 0;

/**
  * 简介：该函数用于MCU对LoRaWAN节点模块的串口初始化。              
  * 参数： 无  
  * 返回值：无
  */
void LoRaNode_UART_Init(void)                                               
{
	uart3_init(9600);
}

/**
  * 简介：该函数为低功耗定时器中断模式开启。              
  * 参数： arr单位为秒， LPTIM_Flag为应用中的超时类型，使用者可以自行增加 
  * 返回值：无
  */
void TIM7_SingleStart_s(uint16_t arr)
{
	TIM_Cmd(TIM7,DISABLE);//关闭外设
	SysTickSCount_flag=0;
	SysTickSCount = arr;
	TIM_Cmd(TIM7,ENABLE);//开启外设
}

//-----------------------------GPIO-----------------------------//
//该部分函数为系统用到的GPIO的初始化函数，用户根据自己的平台相应修改
//--------------------------------------------------------------//
void LoRaNode_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  GPIO_InitStructure.GPIO_Pin = Lora_RST_Pin | Lora_MOD_Pin;    //abcd OE ST LT0输出
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//push-pull
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//
  GPIO_Init(Lora_RST_Port, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = Lora_BUSY_Pin ;    
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_Init(Lora_BUSY_Port, &GPIO_InitStructure);

  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
  GPIO_InitStructure.GPIO_Pin = Lora_WAKE_Pin;    //abcd OE ST LT0输出
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//push-pull
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//
  GPIO_Init(Lora_WAKE_Port, &GPIO_InitStructure);

  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
  GPIO_InitStructure.GPIO_Pin = Lora_STAT_Pin;    
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(Lora_STAT_Port, &GPIO_InitStructure);
}

//----------------------------Delay-----------------------------//
void Delay_ms(uint32_t Delay)
{
    HAL_Delay(Delay);
}



