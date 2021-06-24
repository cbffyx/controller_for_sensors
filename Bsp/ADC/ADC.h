#ifndef __ADC_H
#define __ADC_H	
#include <stm32l1xx.h>

void ADCNODMA_Config(void);

void GetADCValue(uint32_t vbat[]);
#endif 
  

