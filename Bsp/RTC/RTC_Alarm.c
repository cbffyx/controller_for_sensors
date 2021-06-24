/**
  * @brief  RTC���� program.
  * @description None
  * @param  teper
  */


#include "protocal.h"
#include "RTC_Alarm.h"
//#include "exti.h"
#include "stdio.h"
#include "usart.h"
#include "time.h"
//#define RTC_CLOCK_SOURCE_LSI
#define RTC_CLOCK_SOURCE_LSE
//__IO uint32_t AsynchPrediv = 0, SynchPrediv = 0;

static uint16_t exti_cnt;
extern uint16_t daytime;
extern uint16_t Dfrequency;

uint16_t get_exti_cnt(void)
{
    return exti_cnt;
}
void reset_exti_cnt(void)
{
    exti_cnt=0;
}
void set_exti_cnt(uint16_t tmp)
{
    exti_cnt=tmp;
}
//uint16_t EXTIcnt;
void RTC_Config(void)
{
    /* Enable the PWR clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

    /* Allow access to RTC */
    PWR_RTCAccessCmd(ENABLE);

    /*!< Reset RTC Domain */
    RCC_RTCResetCmd(ENABLE);
    RCC_RTCResetCmd(DISABLE);

#if defined (RTC_CLOCK_SOURCE_LSI)  /* LSI used as RTC source clock*/
    /* The RTC Clock may varies due to LSI frequency dispersion. */
    /* Enable the LSI OSC */
    RCC_LSICmd(ENABLE);

    /* Wait till LSI is DEV_STA_NOT_CALIBED */
    while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
    {
    }

    /* Select the RTC Clock Source */
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);

#elif defined (RTC_CLOCK_SOURCE_LSE) /* LSE used as RTC source clock */
    /* Enable the LSE OSC */
    RCC_LSEConfig(RCC_LSE_ON);

    /* Wait till LSE is DEV_STA_NOT_CALIBED */
    while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
    {
    }

    /* Select the RTC Clock Source */
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

#else
//  #error Please select the RTC Clock source inside the main.c file
#endif /* RTC_CLOCK_SOURCE_LSI */

    /* Enable the RTC Clock */
    RCC_RTCCLKCmd(ENABLE);

    /* Wait for RTC APB registers synchronisation */
    RTC_WaitForSynchro();

    RTC_TimeStampCmd(RTC_TimeStampEdge_Rising, DISABLE);  //RTC->CR TSEλ��0 �ر����ּ��
    RTC_TimeStampCmd(RTC_TimeStampEdge_Falling, DISABLE); //RTC->CR TSEλ��0 �ر����ּ��
}

void RTC_Alarm_Configuration(void)
{
    NVIC_InitTypeDef  NVIC_InitStructure;
    EXTI_InitTypeDef  EXTI_InitStructure;

    RTC_Config();

    RTC_TamperCmd(RTC_Tamper_1, DISABLE);
    RTC_TamperCmd(RTC_Tamper_2, DISABLE);
    RTC_TamperCmd(RTC_Tamper_3, DISABLE);
    PWR_UltraLowPowerCmd(ENABLE);
    PWR_FastWakeUpCmd(ENABLE);

    /* EXTI configuration *******************************************************/
    EXTI_ClearITPendingBit(EXTI_Line20);
    EXTI_InitStructure.EXTI_Line = EXTI_Line20;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    /* Enable the RTC Wakeup Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* RTC Wakeup Interrupt Generation: Clock Source: RTCDiv_16, Wakeup Time Base: 3s */    //0x1FFF 3s
    RTC_WakeUpClockConfig(RTC_WakeUpClock_RTCCLK_Div16);//32768Hz/16=2048Hz

#if defined (RTC_CLOCK_SOURCE_LSI)  /* LSI used as RTC source clock*/
    RTC_SetWakeUpCounter(0xb4a0);//46240
    // 23125/(37k/16)=10s 0x5a56; 20s (37k / 16)*20-1 0xb4a0

#elif defined (RTC_CLOCK_SOURCE_LSE) /* LSE used as RTC source clock */
    RTC_SetWakeUpCounter(0x9f1c);//40723
    //
    //16Div 20480/(32.768k/16)=10s 0x5000;
    //20s (32.768k/16)*20 40960 0xA000 -1=0x9FFF
    //30s (32.768k/16)*30 61440 0xF000 -1=0xEFFF -0x153=0xEEAC
#endif /* RTC_CLOCK_SOURCE */
    /* Enable the Wakeup Interrupt */
    RTC_ITConfig(RTC_IT_WUT, ENABLE);
    /* Enable Wakeup Counter */
    RTC_WakeUpCmd(ENABLE);
}

void RTC_Alarm_Configuration1(uint16_t WaitTimeS)//1-30s
{
		uint16_t TempWaitTime;
	  if (WaitTimeS>30) WaitTimeS=30;
    NVIC_InitTypeDef  NVIC_InitStructure;
    EXTI_InitTypeDef  EXTI_InitStructure;

    RTC_Config();

    RTC_TamperCmd(RTC_Tamper_1, DISABLE);
    RTC_TamperCmd(RTC_Tamper_2, DISABLE);
    RTC_TamperCmd(RTC_Tamper_3, DISABLE);
    PWR_UltraLowPowerCmd(ENABLE);
    PWR_FastWakeUpCmd(ENABLE);

    /* EXTI configuration *******************************************************/
    EXTI_ClearITPendingBit(EXTI_Line20);
    EXTI_InitStructure.EXTI_Line = EXTI_Line20;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    /* Enable the RTC Wakeup Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* RTC Wakeup Interrupt Generation: Clock Source: RTCDiv_16, Wakeup Time Base: 3s */    //0x1FFF 3s
    RTC_WakeUpClockConfig(RTC_WakeUpClock_RTCCLK_Div16);//32768Hz/16=2048Hz

#if defined (RTC_CLOCK_SOURCE_LSI)  /* LSI used as RTC source clock*/
    RTC_SetWakeUpCounter(0xb4a0);//46240
    // 23125/(37k/16)=10s 0x5a56; 20s (37k / 16)*20-1 0xb4a0

#elif defined (RTC_CLOCK_SOURCE_LSE) /* LSE used as RTC source clock */
		TempWaitTime=WaitTimeS*2048-228;
    RTC_SetWakeUpCounter(TempWaitTime);//40723
    //
    //16Div 20480/(32.768k/16)=10s 0x5000;
    //20s (32.768k/16)*20 40960 0xA000 -1=0x9FFF
    //30s (32.768k/16)*30 61440 0xF000 -1=0xEFFF -0x153=0xEEAC
#endif /* RTC_CLOCK_SOURCE */
    /* Enable the Wakeup Interrupt */
    RTC_ITConfig(RTC_IT_WUT, ENABLE);
    /* Enable Wakeup Counter */
    RTC_WakeUpCmd(ENABLE);
}
/*******************************************************************************
* @Function Name	:RTC�����жϼ����Ѽ���
* @Description	:  .
* @Input		: None
* @Output	: None
* @Return	: None
*******************************************************************************/
void RTC_WKUP_IRQHandler(void)
{
    if(RTC_GetITStatus(RTC_IT_WUT) != RESET)
    {
        exti_cnt++; //per 20s
        daytime++;
if (Dfrequency<0x8000)				
        SystimeAdd20();
        if (exti_cnt<1100)
        {
            IWDG_ReloadCounter();//ι�� //ע��ȡֵ�������������������������Ĺ������Ϊ12Сʱ
        }
        RTC_ClearITPendingBit(RTC_IT_WUT);
        EXTI_ClearITPendingBit(EXTI_Line20);
        RTC_Alarm_Configuration();
    }
}


void xSeconds2Date(unsigned long seconds,RTC_DateTypeDef *date)
{
    static unsigned int month[12]= {
        /*01��*/31,
        /*02��*/28,
        /*03��*/31,
        /*04��*/30,
        /*05��*/31,
        /*06��*/30,
        /*07��*/31,
        /*08��*/31,
        /*09��*/30,
        /*10��*/31,
        /*11��*/30,
        /*12��*/31
    };
    unsigned int days;
    unsigned short leap_y_count;
    seconds          /= 60;
    seconds          += 8 * 60 ;        //ʱ������ תΪUTC+8 bylzs
    seconds          /= 60;
    days              = seconds / 24;//���������
    leap_y_count = (days + 365) / 1461;//��ȥ�˶��ٸ�����(4��һ��)
    if( ((days + 366) % 1461) == 0)
    {   //��������1��
        date->RTC_Year = 1970 + (days / 366);//�����
        date->RTC_Month = 12;              //������
        date->RTC_Date = 31;
        return;
    }
    days -= leap_y_count;
    date->RTC_Year = 1970 + (days / 365);     //�����
    days %= 365;                       //����ĵڼ���
    days = 01 + days;                  //1�տ�ʼ
    if( (date->RTC_Year % 4) == 0 )
    {
        if(days > 60)--days;            //�������
        else
        {
            if(days == 60)
            {
                date->RTC_Month = 2;
                date->RTC_Date = 29;
                return;
            }
        }
    }
    for(date->RTC_Month = 0; month[date->RTC_Month] < days; date->RTC_Month++)
    {
        days -= month[date->RTC_Month];
    }
    ++date->RTC_Month;               //������
    date->RTC_Date = days;           //�����
}

void Seconds2Date(unsigned long seconds,RTC_DateTypeDef *date)
{
    struct tm gm_date;
    gm_date = *localtime((time_t*)(&seconds));
    date->RTC_Year = gm_date.tm_year;
    date->RTC_Month = gm_date.tm_mon;
    date->RTC_Date = gm_date.tm_mday ;
}

unsigned long Date2Seconds(RTC_DateTypeDef *date,RTC_TimeTypeDef *time)
{
    struct tm gm_date;
    gm_date.tm_year = date->RTC_Year;
    gm_date.tm_mon = date->RTC_Month;
    gm_date.tm_mday = date->RTC_Date;
    gm_date.tm_hour = time->RTC_Hours;
    gm_date.tm_mon = time->RTC_Minutes;
    gm_date.tm_sec = time->RTC_Seconds;
    return mktime(&gm_date);
}