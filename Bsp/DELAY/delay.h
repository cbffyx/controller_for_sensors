#ifndef __DELAY_H
#define __DELAY_H 			   
#include <stm32l1xx.h>
void delay_init(void);
void delay_ms(uint16_t nms);
void delay_us(uint16_t nus);
void delay_min(uint8_t nmin);
void delay_second(uint8_t ns);
#endif

