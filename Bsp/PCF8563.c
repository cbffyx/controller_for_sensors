#include "my_type.h"


PCF8563_TimeTypeDef   RTC_TimeStr={0,0,0,0,0,0};
PCF8563_AlarmTypeDef  RTC_AlarmStr={0,0,0};
//-----------------------------------------------------------------------------
etError PCF8563_Config(void)
{
    etError error;  // error code
    
    BSP.tmp[1]  = 0x02;             //报警中断有效    
    BSP.tmp[11] = 0x80;             //日报警无效
    BSP.tmp[12] = 0x80;             //星期报警无效
    BSP.tmp[13] = 0x00;
    BSP.tmp[14] = 0x00;
    BSP.tmp[15] = 0x00;
    error = I2C_WriteNbyte(PCF8563,8,0x01,&BSP.tmp[1],1);   
    if(error != NO_ERROR)
    {
        error |= I2C_WriteNbyte(PCF8563,8,0x01,&BSP.tmp[1],1);   
    }
    
    error = I2C_WriteNbyte(PCF8563,8,0x0B,&BSP.tmp[11],5);   
    if(error != NO_ERROR)
    {
        error |= I2C_WriteNbyte(PCF8563,8,0x0B,&BSP.tmp[11],5);   
    }
    return error;
}

//-----------------------------------------------------------------------------
etError PCF8563_GetTime(PCF8563_Format_TypeDef RTC_Format,PCF8563_TimeTypeDef* RTC_TimeStruct)
{
    etError error;  // error code
    
    error = I2C_ReadNbyte(PCF8563,8,0x02,&BSP.tmp[2],7); 
    if(error != NO_ERROR)
    {
        error |= I2C_ReadNbyte(PCF8563,8,0x02,&BSP.tmp[2],7);
    }
    
    if(0x80==(BSP.tmp[2]&0x80)) 
    {
        BSP.tmp[2] &= 0x7f;
        I2C_WriteNbyte(PCF8563,8,0x02,&BSP.tmp[2],1); 
    }
        
    BSP.tmp[2] = BSP.tmp[2] & 0x7f;
    BSP.tmp[3] = BSP.tmp[3] & 0x7f;
    BSP.tmp[4] = BSP.tmp[4] & 0x3f;
    BSP.tmp[5] = BSP.tmp[5] & 0x3f;
    BSP.tmp[6] = 1;
    BSP.tmp[7] = BSP.tmp[7] & 0x1f;
    BSP.tmp[8] = BSP.tmp[8] & 0xff;  
    
    if(RTC_Format == Format_BIN)
    {
        RTC_TimeStruct->RTC_Year = (uint8_t)Bcd2ToByte(BSP.tmp[8]);
        RTC_TimeStruct->RTC_Month = (uint8_t)Bcd2ToByte(BSP.tmp[7]);
        RTC_TimeStruct->RTC_Date = (uint8_t)Bcd2ToByte(BSP.tmp[5]);   
        RTC_TimeStruct->RTC_Hours = (uint8_t)Bcd2ToByte(BSP.tmp[4]);
        RTC_TimeStruct->RTC_Minutes = (uint8_t)Bcd2ToByte(BSP.tmp[3]);
        RTC_TimeStruct->RTC_Seconds = (uint8_t)Bcd2ToByte(BSP.tmp[2]);
    }
    else
    {
        RTC_TimeStruct->RTC_Year = BSP.tmp[8];
        RTC_TimeStruct->RTC_Month = BSP.tmp[7];
        RTC_TimeStruct->RTC_Date = BSP.tmp[5];
        RTC_TimeStruct->RTC_Hours = BSP.tmp[4];
        RTC_TimeStruct->RTC_Minutes = BSP.tmp[3];
        RTC_TimeStruct->RTC_Seconds = BSP.tmp[2];
    }
    return error;
}

//-----------------------------------------------------------------------------
etError PCF8563_SetTime(PCF8563_Format_TypeDef RTC_Format,PCF8563_TimeTypeDef* RTC_TimeStruct)
{
    etError error;  // error code
    
    if (RTC_Format == Format_BIN)
    {
        BSP.tmp[8] = (uint8_t)ByteToBcd2(RTC_TimeStruct->RTC_Year);
        BSP.tmp[7] = (uint8_t)ByteToBcd2(RTC_TimeStruct->RTC_Month);
        BSP.tmp[6] = 1;
        BSP.tmp[5] = (uint8_t)ByteToBcd2(RTC_TimeStruct->RTC_Date);
        BSP.tmp[4] = (uint8_t)ByteToBcd2(RTC_TimeStruct->RTC_Hours);
        BSP.tmp[3] = (uint8_t)ByteToBcd2(RTC_TimeStruct->RTC_Minutes);
        BSP.tmp[2] = (uint8_t)ByteToBcd2(RTC_TimeStruct->RTC_Seconds);
    }
    else
    {
        BSP.tmp[8] = RTC_TimeStruct->RTC_Year;
        BSP.tmp[7] = RTC_TimeStruct->RTC_Month;
        BSP.tmp[6] = 1;
        BSP.tmp[5] = RTC_TimeStruct->RTC_Date;
        BSP.tmp[4] = RTC_TimeStruct->RTC_Hours;
        BSP.tmp[3] = RTC_TimeStruct->RTC_Minutes;
        BSP.tmp[2] = RTC_TimeStruct->RTC_Seconds;
    }  
    
    BSP.tmp[2] = BSP.tmp[2] & 0x7f;
    BSP.tmp[3] = BSP.tmp[3] & 0x7f;
    BSP.tmp[4] = BSP.tmp[4] & 0x3f;
    BSP.tmp[5] = BSP.tmp[5] & 0x3f;
    BSP.tmp[7] = BSP.tmp[7] & 0x1f;
    BSP.tmp[8] = BSP.tmp[8] & 0xff;    
 
    error = I2C_WriteNbyte(PCF8563,8,0x02,&BSP.tmp[2],7);   
    if(error != NO_ERROR)
    {
        error |= I2C_WriteNbyte(PCF8563,8,0x02,&BSP.tmp[2],7); 
    }
    return error;
}

//-----------------------------------------------------------------------------
etError PCF8563_GetAlarm(PCF8563_Format_TypeDef RTC_Format,PCF8563_AlarmTypeDef* RTC_AlarmStruct)
{
    etError error;  // error code
    
    error = I2C_ReadNbyte(PCF8563,8,0x09,&BSP.tmp[9],2); 
    if(error != NO_ERROR)
    {
        error |= I2C_ReadNbyte(PCF8563,8,0x09,&BSP.tmp[9],2); 
    }
    
    BSP.tmp[9]  = BSP.tmp[9]  & 0x7f;     //fen
    BSP.tmp[10] = BSP.tmp[10] & 0x3f;     //shi
    
    if (RTC_Format == Format_BIN)
    {
        RTC_AlarmStruct->RTC_Hours = (uint8_t)Bcd2ToByte(BSP.tmp[10]);
        RTC_AlarmStruct->RTC_Minutes = (uint8_t)Bcd2ToByte(BSP.tmp[9]);
    }
    else
    {
        RTC_AlarmStruct->RTC_Hours = BSP.tmp[10];
        RTC_AlarmStruct->RTC_Minutes = BSP.tmp[9];
    }  
    return error;
}

//-----------------------------------------------------------------------------
etError PCF8563_SetAlarm(PCF8563_Format_TypeDef RTC_Format,PCF8563_AlarmTypeDef* RTC_AlarmStruct)
{
    etError error;  // error code
    
    if (RTC_Format == Format_BIN)
    {
        BSP.tmp[10] = (uint8_t)ByteToBcd2(RTC_AlarmStruct->RTC_Hours);
        BSP.tmp[9]  = (uint8_t)ByteToBcd2(RTC_AlarmStruct->RTC_Minutes);
    }
    else
    {
        BSP.tmp[10] = RTC_AlarmStruct->RTC_Hours;
        BSP.tmp[9]  = RTC_AlarmStruct->RTC_Minutes;
    } 
    BSP.tmp[9]  = BSP.tmp[9]  & 0x7f;     //fen
    BSP.tmp[10] = BSP.tmp[10] & 0x3f;     //shi
    BSP.tmp[11] = 0x80;               //日报警无效
    BSP.tmp[12] = 0x80;               //星期报警无效
    BSP.tmp[13] = 0x00;               //时钟输出无效
    BSP.tmp[14] = 0x00;               //定时报警无效
    error = I2C_WriteNbyte(PCF8563,8,0x09,&BSP.tmp[9],6);   
    if(error != NO_ERROR)
    {
        error |= I2C_WriteNbyte(PCF8563,8,0x09,&BSP.tmp[9],6); 
    }
    return error;
}


/*******************************************************************************
****函数备注：应用层
*******************************************************************************/
void EXTI_TIME_INT(void)                            //时钟中断初始化
{
    GPIO_Init(INT_PORT, INT_PINS, GPIO_Mode_In_PU_IT);       //8563中断   
    EXTI_SetPinSensitivity (EXTI_Pin_2,EXTI_Trigger_Falling);//外部中断pb2，下降沿触发  
}

//-----------------------------------------------------------------------------
void Time_Config(void)                              //RTC初始化
{   
    uint8_t num=0;
    
    PCF8563_Config();

    while(NO_ERROR != PCF8563_GetTime(Format_BIN, &RTC_TimeStr));   //获取RTC当前时间 
    while(NO_ERROR != PCF8563_GetAlarm(Format_BIN, &RTC_AlarmStr)); //读取闹钟 
    while((RTC_TimeStr.RTC_Year<17)||(RTC_TimeStr.RTC_Year>99)||
          (RTC_TimeStr.RTC_Month<1)||(RTC_TimeStr.RTC_Month>12)||
          (RTC_TimeStr.RTC_Date<1) ||(RTC_TimeStr.RTC_Date>31))
    {
        if(++num > 5) {
            WWDG->CR = 0x80;                        //复位
        }    
        RTC_TimeStr.RTC_Year  = 18;
        RTC_TimeStr.RTC_Month = 1;
        RTC_TimeStr.RTC_Date  = 20;    
        RTC_TimeStr.RTC_Hours   = 12;
        RTC_TimeStr.RTC_Minutes = 30;
        RTC_TimeStr.RTC_Seconds = 00;
        while(NO_ERROR != PCF8563_SetTime(Format_BIN, &RTC_TimeStr));//设置时间  
        while(NO_ERROR != PCF8563_GetTime(Format_BIN, &RTC_TimeStr));//获取RTC当前时间   
        
        RTC_AlarmStr.RTC_Hours   = 0;
        RTC_AlarmStr.RTC_Minutes = 0;

        while(NO_ERROR != PCF8563_SetAlarm(Format_BIN, &RTC_AlarmStr));//设置闹钟   
        while(NO_ERROR != PCF8563_GetAlarm(Format_BIN, &RTC_AlarmStr));//读取闹钟 
    }
}

//-----------------------------------------------------------------------------
void Time_ClockCmd(uint8_t *p)                      //设置时钟
{
    uint8_t num=0;
    
    while(1)
    {
        if(++num > 5) {
            WWDG->CR = 0x80;                        //复位
        }
        
        RTC_TimeStr.RTC_Year  = p[0];
        RTC_TimeStr.RTC_Month = p[1];
        RTC_TimeStr.RTC_Date  = p[2];
        RTC_TimeStr.RTC_Hours   = p[3];
        RTC_TimeStr.RTC_Minutes = p[4];
        RTC_TimeStr.RTC_Seconds = p[5];
        
        PCF8563_SetTime(Format_BIN, &RTC_TimeStr);  //设置时间        
        PCF8563_GetTime(Format_BIN, &RTC_TimeStr);  //获取RTC当前时间
        
        if(RTC_TimeStr.RTC_Year!=p[0])      continue;
        if(RTC_TimeStr.RTC_Month!=p[1])     continue;
        if(RTC_TimeStr.RTC_Date!=p[2])      continue;
        if(RTC_TimeStr.RTC_Hours!=p[3])     continue;
        if(RTC_TimeStr.RTC_Minutes!=p[4])   continue;
        if(RTC_TimeStr.RTC_Seconds!=p[5])   continue;
        
        return;
    }    
}

//-----------------------------------------------------------------------------
void Time_AlarmCmd(uint8_t Hou,uint8_t Min)         //设置闹钟
{
    uint8_t num=0;

    while(1)
    {
        if(++num > 5) {
            WWDG->CR = 0x80;                        //复位
        }
        PCF8563_GetAlarm(Format_BIN, &RTC_AlarmStr);//读取闹钟
        if((RTC_AlarmStr.RTC_Hours == Hou)&&(RTC_AlarmStr.RTC_Minutes == Min)) return;

        RTC_AlarmStr.RTC_Hours   = Hou;
        RTC_AlarmStr.RTC_Minutes = Min;

        PCF8563_SetAlarm(Format_BIN, &RTC_AlarmStr);//设置闹钟   
        PCF8563_GetAlarm(Format_BIN, &RTC_AlarmStr);//读取闹钟 
        
        if(RTC_AlarmStr.RTC_Hours!=Hou)   continue;  
        if(RTC_AlarmStr.RTC_Minutes!=Min) continue;
        
        return;
    }
}