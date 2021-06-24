#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include <stm32l1xx.h>
#define u8 int8_t
#define u16 int16_t
#define u32 int32_t


#define usart3RxBufferSize 200
#define usart2RxBufferSize 100
#define usart1RxBufferSize 250

void uart1_init(u32 bound);
void uart2_init(u32 bound);
void uart3_init(u32 bound);
void Uart1_SendStr(char*SendBuf);
void Uart2_SendStr(char*SendBuf);
void Uart3_SendStr(char*SendBuf);
void Uart1_SendData(char *data, uint16_t len);
void Uart2_SendData(char *data, uint16_t len);
void Uart3_SendData(char *data, uint16_t len);
void UART1_send_byte(char data);
void UART2_send_byte(char data);
void UART3_send_byte(char data);
void Uart1_SendHEXData(char *data, uint16_t len);

void PrintUsart3Buffer(void);
void PrintClrUsart3Buffer(void);
void PrintUsart2Buffer(void);
void PrintClrUsart2Buffer(void);

void ClrUsart3Buffer(void);

extern char usart1RxBuffer[usart1RxBufferSize];     
extern uint16_t usart2RxCnt;
extern uint16_t usart1RxCnt;
//extern char usart2RxBuffer[usart2RxBufferSize];     
extern uint16_t usart3RxCnt; 
extern char usart3RxBuffer[usart3RxBufferSize];    

#define DEBUG_SendStr Uart1_SendStr
#define DEBUG_RxBuffer usart1RxBuffer
#define DEBUG_RxCnt usart1RxCnt
#endif


