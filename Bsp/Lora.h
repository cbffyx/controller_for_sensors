#ifndef __Lora_H__
#define __Lora_H__
#include <stm32l1xx.h>
#include "protocal.h"
#include "TIM.h"
#include "M90_M91_HAL.h"
#include "M90_M91_Interface.h"
#include "main.h"
#include "delay.h"
#include "string.h"
#include "SoftwareReset.h"
#include "Common.h"
#include "usart.h"
#include "ProUart1.h"	

#define LoraRxBuffer usart3RxBuffer
#define LoraRxCnt usart3RxCnt
#define LoraRxBufferSize usart3RxBufferSize
#define PrintLoraBuffer PrintUsart3Buffer
#define PrintClrLoraBuffer PrintClrUsart3Buffer
#define ClrLoraBuffer	ClrUsart3Buffer
#define HAL_Delay delay_ms

#define LoraTimeOutDefault 3000

uint8_t Time_Out_Break(uint32_t MAX_time , uint8_t *Sign);
int Lora_Init(void);
int LoRaWAN_Node_Send(uint8_t *buf, uint16_t len, uint8_t confirm);
void Lora_GPIO_Init(void);
int SyncGetTime(void);
#endif 

