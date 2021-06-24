/**
******************************************************************************
* @file    M90_M91_HAL.c
* @author  LSD RF/LoRaWAN Team
* @version V0.0.0
* @date    2-April-2018
* @brief   This file contains all the functions prototypes for the HAL
*          module driver.
*/

#ifndef __M90_M91_HAL_H
#define __M90_M91_HAL_H

#include <stdint.h>
#include "usart.h"
#include "TIM.h"
#include "delay.h"
#include "main.h"


/* LoRawan LoRaNode pin configuration */
#define Lora_RST_Pin GPIO_Pin_11
#define Lora_RST_Port GPIOA
#define Lora_WAKE_Pin GPIO_Pin_9
#define Lora_WAKE_Port GPIOB
#define Lora_MOD_Pin GPIO_Pin_0
#define Lora_MOD_Port GPIOA

#define Lora_BUSY_Pin GPIO_Pin_15
#define Lora_BUSY_Port GPIOA

#define Lora_STAT_Pin GPIO_Pin_13
#define Lora_STAT_Port GPIOC

#define Lora_TXD_Pin GPIO_Pin_10
#define Lora_TXD_Port GPIOB
#define Lora_RXD_Pin GPIO_Pin_11
#define Lora_RXD_Port GPIOB

#define LoRaNode_WAKE_PIN					Lora_WAKE_Pin
#define LoRaNode_WAKE_GPIO_PORT         	Lora_WAKE_Port

#define LoRaNode_MODE_PIN					Lora_MOD_Pin
#define LoRaNode_MODE_GPIO_PORT         	Lora_MOD_Port

#define LoRaNode_NRST_PIN					Lora_RST_Pin
#define LoRaNode_NRST_GPIO_PORT         	Lora_RST_Port

/* INPUT */													
#define LoRaNode_STAT_PIN					Lora_STAT_Pin
#define LoRaNode_STAT_GPIO_PORT         	Lora_STAT_Port

#define LoRaNode_BUSY_PIN					Lora_BUSY_Pin
#define LoRaNode_BUSY_GPIO_PORT         	Lora_BUSY_Port

#define RECEIVELEN usart3RxBufferSize

#define HAL_Delay delay_ms

typedef struct  
{  
    uint8_t receive_flag;						
    uint16_t rx_len;			
    uint8_t RX_Buf[RECEIVELEN];          
}LoRaNode_USART_RECEIVETYPE; 

typedef enum  
{  
    LoRaNode_ReadTimeout=0,			
    LoRaNode_BUSYTimeout,
    LoRaNode_LPsendTimeout,			  
    reserve,            
}LPTIM1_FlagTypeDef;

extern uint8_t LoRaNode_ReadTimeout_flag,LoRaNode_BUSYTimeout_flag,LoRaNode_LPsend_flag;

//-----------------------------UART-----------------------------//
void LoRaNode_UART_Init(void);


//----------------------------LPTIM1-----------------------------//
void TIM7_SingleStart_s(uint16_t arr);


//-----------------------------GPIO-----------------------------//
void LoRaNode_GPIO_Init(void);

//----------------------------Delay-----------------------------//
void Delay_ms(uint32_t Delay);

#endif

