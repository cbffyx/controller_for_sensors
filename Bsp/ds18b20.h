#ifndef _DS18B20_H_
#define _DS18B20_H_


//-----------------------------------------------------------------------------
#define DS18B20_DQ_OUT  GPIO_Init(DQ_PORT, DQ_PINS, GPIO_Mode_Out_PP_High_Fast)
#define DS18B20_DQ_IN   GPIO_Init(DQ_PORT, DQ_PINS, GPIO_Mode_In_PU_No_IT)
#define DS18B20_DQ_High GPIO_SetBits(DQ_PORT,DQ_PINS)
#define DS18B20_DQ_Low  GPIO_ResetBits(DQ_PORT,DQ_PINS) 

#define get_DQ()    ( 0 == GPIO_ReadInputDataBit( DQ_PORT, DQ_PINS )) ? 0 : 1 // ∂¡»° ‰»Î◊¥Ã¨

//-----------------------------------------------------------------------------
float  DS18B20(void);


#endif