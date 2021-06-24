
#ifndef __RTC_Alarm_H
#define __RTC_Alarm_H

#include <stm32l1xx.h>

void RTC_Alarm_Configuration(void);
void RTC_Alarm_Configuration1(uint16_t WaitTimeS);
void reset_exti_cnt(void);
uint16_t get_exti_cnt(void);
void set_exti_cnt(uint16_t tmp);
void xSeconds2Date(unsigned long seconds,RTC_DateTypeDef *time);
void Seconds2Date(unsigned long seconds,RTC_DateTypeDef *date);
unsigned long Date2Seconds(RTC_DateTypeDef *date,RTC_TimeTypeDef *time);

//extern uint16_t EXTIcnt;
#endif 

