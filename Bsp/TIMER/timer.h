#ifndef __TIMER_H
#define __TIMER_H
#include <stm32l1xx.h>
#include "usart.h"


void TIM3_Int_Init(u16 arr,u16 psc);

#endif
