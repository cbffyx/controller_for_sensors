/**
  ******************************************************************************
  * �ļ��� ��   M90_M91_HAL.c
  * ����   ��   LSD RF/LoRaWAN Team
  * �汾   ��   V1.0.0
  * ʱ��   ��   19-May-2018
  * �ļ�������
  *     ���ļ�ΪM90_M91ģ���Ӳ���㣬����MCU��M90ģ��Ĵ������ã�GPIO�ڳ�ʼ������
  *�����ڸ�����ʱ�жϵĵ͹���ʱ�ӵȡ�
  *    �ͻ���ʹ��M90ģ��ʱ����Ҫ��ֲ���ļ�����֤�����������������ββ���������
  *�����Լ���MCUƽ̨�޸ĺ������ݣ�ʹ�������ܿ��������С�Ӳ����ռ����Դ���£�
  *
  *���ڣ�������ʹ��STM32L4�ĵ͹��Ĵ���LPUART1��M90ģ�����ͨ�š�
  *��ʱ����������ʹ����STM32L4��TIM3��Ϊ���ڵ�֡��β�жϡ�
  *        ʹ����STM32L4��LPTIM1��ΪӦ�ò���Ҫ�ĸ�����ʱ��ʱ����
  *        ����û��������ط���Ҫ�õ���������ʱ������ӿڶ����������û����ڻص���
  *        �����������
  *GPIO�ڣ�������ʹ�õ�GPIO���������£�
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
  * ��飺�ú�������MCU��LoRaWAN�ڵ�ģ��Ĵ��ڳ�ʼ����              
  * ������ ��  
  * ����ֵ����
  */
void LoRaNode_UART_Init(void)                                               
{
	uart3_init(9600);
}

/**
  * ��飺�ú���Ϊ�͹��Ķ�ʱ���ж�ģʽ������              
  * ������ arr��λΪ�룬 LPTIM_FlagΪӦ���еĳ�ʱ���ͣ�ʹ���߿����������� 
  * ����ֵ����
  */
void TIM7_SingleStart_s(uint16_t arr)
{
	TIM_Cmd(TIM7,DISABLE);//�ر�����
	SysTickSCount_flag=0;
	SysTickSCount = arr;
	TIM_Cmd(TIM7,ENABLE);//��������
}

//-----------------------------GPIO-----------------------------//
//�ò��ֺ���Ϊϵͳ�õ���GPIO�ĳ�ʼ���������û������Լ���ƽ̨��Ӧ�޸�
//--------------------------------------------------------------//
void LoRaNode_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  GPIO_InitStructure.GPIO_Pin = Lora_RST_Pin | Lora_MOD_Pin;    //abcd OE ST LT0���
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
  GPIO_InitStructure.GPIO_Pin = Lora_WAKE_Pin;    //abcd OE ST LT0���
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



