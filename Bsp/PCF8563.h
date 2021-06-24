#ifndef _PCF8563_H_
#define _PCF8563_H_


//-----------------------------------------------------------------------------
typedef struct
{        
    uint8_t RTC_Year;   
    uint8_t RTC_Month;  
    uint8_t RTC_Date;     
    uint8_t RTC_Hours;     
    uint8_t RTC_Minutes;              
    uint8_t RTC_Seconds;        
}PCF8563_TimeTypeDef;
extern PCF8563_TimeTypeDef   RTC_TimeStr;

typedef struct
{
    uint8_t RTC_Hours;     
    uint8_t RTC_Minutes;              
    uint8_t RTC_Seconds;                                 
}PCF8563_AlarmTypeDef;
extern PCF8563_AlarmTypeDef  RTC_AlarmStr;

typedef enum
{
  Format_BIN  = ((uint8_t)0x00), /*!< Binary Format is used  */
  Format_BCD  = ((uint8_t)0x01)  /*!< BCD Format is used  */
}PCF8563_Format_TypeDef;

//-----------------------------------------------------------------------------

#define     PCF8563   0xA2 

//-----------------------------------------------------------------------------
etError PCF8563_Config(void);
etError PCF8563_SetTime(PCF8563_Format_TypeDef RTC_Format,PCF8563_TimeTypeDef* RTC_TimeStruct);
etError PCF8563_GetTime(PCF8563_Format_TypeDef RTC_Format,PCF8563_TimeTypeDef* RTC_TimeStruct);
etError PCF8563_SetAlarm(PCF8563_Format_TypeDef RTC_Format,PCF8563_AlarmTypeDef* RTC_AlarmStruct);
etError PCF8563_GetAlarm(PCF8563_Format_TypeDef RTC_Format,PCF8563_AlarmTypeDef* RTC_AlarmStruct);

void EXTI_TIME_INT(void);
void Time_Config(void);
void Time_ClockCmd(uint8_t *p);
void Time_AlarmCmd(uint8_t Hou,uint8_t Min);


#endif