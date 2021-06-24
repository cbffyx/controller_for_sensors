#ifndef __SPMOTOR_H__
#define __SPMOTOR_H__
#include "delay.h"
#include <stm32l1xx.h>
void Motor_GPIO_Init(void);
void Motor_ccw(uint32_t circle,uint16_t nms);//80 	//12每圈
void Motor_cw(uint32_t circle, uint16_t nms);//130 

#endif
