#ifndef __TIM_H
#define __TIM_H

#include "stm32l1xx.h"

void TIM2_Init_s(void);//BaseUse
void TIM3_UART_Init_ms(uint16_t arr);
void TIM4_UART_Init_ms(uint16_t arr);
void TIM6_UART_Init_ms(uint16_t arr);
void TIM7_Init_s(void);

extern volatile uint8_t LoRaNode_UART_receive_flag;
extern volatile uint8_t BlueTooth_UART_receive_flag;
extern volatile uint8_t Debug_UART_receive_flag;
extern volatile uint32_t SysTickSCount;
extern volatile uint32_t SysTickCount;
extern volatile uint8_t SysTickSCount_flag;

extern uint32_t HAL_GetTick(void);
#endif
