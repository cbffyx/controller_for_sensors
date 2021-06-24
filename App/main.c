#include "main.h"


/*******************************************************************************
****������ע��ʱ�����ã���������ʱ��Ϊ�ⲿHSE��11.0592MHz��������͹��ĺ�ʹ��LSE
*******************************************************************************/
void CLK_Config(FunctionalState NewState)
{  
    if (NewState != DISABLE)
    {
        CLK_HSICmd(DISABLE);                            //�ر��ڲ�����RC 
        CLK_LSICmd(DISABLE);                            //�ر��ڲ�����RC
        CLK_HSEConfig(CLK_HSE_ON);
        while (CLK_GetFlagStatus(CLK_FLAG_HSERDY) == RESET);
        
        CLK_SYSCLKSourceSwitchCmd(ENABLE);              //ʹ���л�  
        CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_HSE);   //ϵͳ��ʱ��ΪHSE           
        while(CLK_GetSYSCLKSource() != CLK_SYSCLKSource_HSE);     
        CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_1);           //1��Ƶ,11.0592       
        
        CLK_PeripheralClockConfig (CLK_Peripheral_ADC1,ENABLE);
        CLK_PeripheralClockConfig (CLK_Peripheral_TIM1,ENABLE);    
        CLK_PeripheralClockConfig (CLK_Peripheral_TIM2,ENABLE);     
        CLK_PeripheralClockConfig (CLK_Peripheral_USART1,ENABLE);  
    }
    else
    {
        CLK_HSICmd(DISABLE);                            //�ر��ڲ�����RC
        CLK_LSICmd(ENABLE);                             //���ڲ�����RC
        while (CLK_GetFlagStatus(CLK_FLAG_LSIRDY) == RESET);
        
        CLK_SYSCLKSourceSwitchCmd(ENABLE);              //ʹ��ʱ���л�
        CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_LSI);   //ѡ���ڲ�����ʱ����Ϊʱ��Դ 
        while (CLK_GetSYSCLKSource() != CLK_SYSCLKSource_LSI);//�ȴ�ʱ���ȶ�
        CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_1);           //����ϵͳʱ�ӷ�Ƶ
        
        CLK_PeripheralClockConfig (CLK_Peripheral_ADC1,DISABLE);
        CLK_PeripheralClockConfig (CLK_Peripheral_TIM1,DISABLE);   
        CLK_PeripheralClockConfig (CLK_Peripheral_TIM2,DISABLE);   
        CLK_PeripheralClockConfig (CLK_Peripheral_USART1,DISABLE);
    }
}

/*******************************************************************************
****������ע��GPIO��ʼ��
*******************************************************************************/
void GPIO_Config(FunctionalState NewState)
{ 
    if (NewState != DISABLE)
    {
        GPIO_Init(GPIOA, GPIO_Pin_All, GPIO_Mode_In_PU_No_IT);
        GPIO_Init(GPIOB, GPIO_Pin_All, GPIO_Mode_In_PU_No_IT);
        GPIO_Init(GPIOC, GPIO_Pin_All, GPIO_Mode_In_PU_No_IT);
        GPIO_Init(GPIOD, GPIO_Pin_All, GPIO_Mode_In_PU_No_IT);
        GPIO_Init(GPIOE, GPIO_Pin_All, GPIO_Mode_In_PU_No_IT);
    }
}

/*******************************************************************************
****������ע��TIME��ʼ��
*******************************************************************************/
void TIME1_Config(void)         //��ʱ��1��ʼ��
{
    TIM1_TimeBaseInit(15,TIM1_CounterMode_Up,(HSE_VALUE/16/100),0);//16��Ƶ�����ϼ�����10ms, 11.0592MHZ
    TIM1_ARRPreloadConfig(ENABLE);          //ʹ���Զ���װ
    TIM1_ITConfig(TIM1_IT_Update , ENABLE); //���ݸ����ж�   
    TIM1_Cmd(ENABLE);           //����ʱ��         
}

void TIME2_Config(void)         //��ʱ��2��ʼ��
{
    TIM2_TimeBaseInit(TIM2_Prescaler_16,TIM2_CounterMode_Up,(HSE_VALUE/16/1000)); //16��Ƶ�����ϼ�����1ms, 11.0592MHZ
    TIM2_ARRPreloadConfig(ENABLE);          //ʹ���Զ���װ
    TIM2_ITConfig(TIM2_IT_Update , ENABLE); //���ݸ����ж�
    TIM2_Cmd(ENABLE);           //����ʱ��            
}

/*******************************************************************************
****������ע��Ӳ��������ʼ������
*******************************************************************************/
void BSP_Configuration(FunctionalState NewState)            
{
    if (NewState != DISABLE)
    {
        CLK_Config(ENABLE);                 //ʱ�ӳ�ʼ��   
        GPIO_Config(ENABLE);                //IO��ʼ��
        //I2C_Config();                       //I2C�˿ڳ�ʼ��
        //EXTI_S2_Sleep();                    //�����жϳ�ʼ��
        //EXTI_TIME_INT();                    //ʱ���жϳ�ʼ��
        //Flash_Config();                     //eeprom  
        //ADC_Config();                       //ADC��ʼ��                   
        //UART1_Config();                     //���ڳ�ʼ��
        //TIME1_Config();                     //��ʱ��1��ʼ��
        //TIME2_Config();                     //��ʱ��2��ʼ��
    }
    else
    {
        ADC_Cmd(ADC1,DISABLE);              //ADC����
        TIM1_Cmd(DISABLE);                  //�ض�ʱ��1      
        TIM2_Cmd(DISABLE);                  //�ض�ʱ��2
        USART_Cmd(USART1,DISABLE);          //��USART1    
        GPIO_Config(DISABLE);               //�͹���IO���� 
        CLK_Config(DISABLE);                //ʹ��LSIϵͳʱ�� 
    }  
}

/*******************************************************************************
****������ע��������
*******************************************************************************/
void main()
{    
    BSP_Configuration(ENABLE);                      //Ӳ��������ʼ������ 
    //enableInterrupts();                             //���ж�  
    while(1)
    {
        get_temp_value();
    }
}

//-----------------------------------------------------------------------------  
#ifdef  USE_FULL_ASSERT

void assert_failed(uint8_t* file, uint32_t line)
{
    while(1)
    {}
}

#endif
