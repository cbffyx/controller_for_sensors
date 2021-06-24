#include "main.h"


/*******************************************************************************
****函数备注：时钟配置，正常工作时钟为外部HSE（11.0592MHz），进入低功耗后使用LSE
*******************************************************************************/
void CLK_Config(FunctionalState NewState)
{  
    if (NewState != DISABLE)
    {
        CLK_HSICmd(DISABLE);                            //关闭内部高速RC 
        CLK_LSICmd(DISABLE);                            //关闭内部低速RC
        CLK_HSEConfig(CLK_HSE_ON);
        while (CLK_GetFlagStatus(CLK_FLAG_HSERDY) == RESET);
        
        CLK_SYSCLKSourceSwitchCmd(ENABLE);              //使能切换  
        CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_HSE);   //系统主时钟为HSE           
        while(CLK_GetSYSCLKSource() != CLK_SYSCLKSource_HSE);     
        CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_1);           //1分频,11.0592       
        
        CLK_PeripheralClockConfig (CLK_Peripheral_ADC1,ENABLE);
        CLK_PeripheralClockConfig (CLK_Peripheral_TIM1,ENABLE);    
        CLK_PeripheralClockConfig (CLK_Peripheral_TIM2,ENABLE);     
        CLK_PeripheralClockConfig (CLK_Peripheral_USART1,ENABLE);  
    }
    else
    {
        CLK_HSICmd(DISABLE);                            //关闭内部高速RC
        CLK_LSICmd(ENABLE);                             //打开内部低速RC
        while (CLK_GetFlagStatus(CLK_FLAG_LSIRDY) == RESET);
        
        CLK_SYSCLKSourceSwitchCmd(ENABLE);              //使能时钟切换
        CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_LSI);   //选择内部低速时钟作为时钟源 
        while (CLK_GetSYSCLKSource() != CLK_SYSCLKSource_LSI);//等待时钟稳定
        CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_1);           //设置系统时钟分频
        
        CLK_PeripheralClockConfig (CLK_Peripheral_ADC1,DISABLE);
        CLK_PeripheralClockConfig (CLK_Peripheral_TIM1,DISABLE);   
        CLK_PeripheralClockConfig (CLK_Peripheral_TIM2,DISABLE);   
        CLK_PeripheralClockConfig (CLK_Peripheral_USART1,DISABLE);
    }
}

/*******************************************************************************
****函数备注：GPIO初始化
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
****函数备注：TIME初始化
*******************************************************************************/
void TIME1_Config(void)         //定时器1初始化
{
    TIM1_TimeBaseInit(15,TIM1_CounterMode_Up,(HSE_VALUE/16/100),0);//16分频，向上计数，10ms, 11.0592MHZ
    TIM1_ARRPreloadConfig(ENABLE);          //使能自动重装
    TIM1_ITConfig(TIM1_IT_Update , ENABLE); //数据更新中断   
    TIM1_Cmd(ENABLE);           //开定时器         
}

void TIME2_Config(void)         //定时器2初始化
{
    TIM2_TimeBaseInit(TIM2_Prescaler_16,TIM2_CounterMode_Up,(HSE_VALUE/16/1000)); //16分频，向上计数，1ms, 11.0592MHZ
    TIM2_ARRPreloadConfig(ENABLE);          //使能自动重装
    TIM2_ITConfig(TIM2_IT_Update , ENABLE); //数据更新中断
    TIM2_Cmd(ENABLE);           //开定时器            
}

/*******************************************************************************
****函数备注：硬件驱动初始化函数
*******************************************************************************/
void BSP_Configuration(FunctionalState NewState)            
{
    if (NewState != DISABLE)
    {
        CLK_Config(ENABLE);                 //时钟初始化   
        GPIO_Config(ENABLE);                //IO初始化
        //I2C_Config();                       //I2C端口初始化
        //EXTI_S2_Sleep();                    //休眠中断初始化
        //EXTI_TIME_INT();                    //时钟中断初始化
        //Flash_Config();                     //eeprom  
        //ADC_Config();                       //ADC初始化                   
        //UART1_Config();                     //串口初始化
        //TIME1_Config();                     //定时器1初始化
        //TIME2_Config();                     //定时器2初始化
    }
    else
    {
        ADC_Cmd(ADC1,DISABLE);              //ADC除能
        TIM1_Cmd(DISABLE);                  //关定时器1      
        TIM2_Cmd(DISABLE);                  //关定时器2
        USART_Cmd(USART1,DISABLE);          //关USART1    
        GPIO_Config(DISABLE);               //低功耗IO设置 
        CLK_Config(DISABLE);                //使能LSI系统时钟 
    }  
}

/*******************************************************************************
****函数备注：主函数
*******************************************************************************/
void main()
{    
    BSP_Configuration(ENABLE);                      //硬件驱动初始化函数 
    //enableInterrupts();                             //开中断  
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
