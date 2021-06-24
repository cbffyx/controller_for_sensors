/**
  ******************************************************************************
  * �ļ��� ��   M90_M91_Interface.c
  * ����   ��   LSD RF/LoRaWAN Team
  * �汾   ��   V1.0.0
  * ʱ��   ��   19-May-2020
  * �ļ�������
  *     ���ļ�ΪM90_M91ģ��������㣬������M90ģ��ĸ���״̬���ƣ�ATָ�����
  *����ͨ�ź����Լ�һЩ���ݴ���Ĺ�������
  *    �ͻ���ʹ��M90ģ��ʱ����Ҫ��ֲ���ļ������M90_M91_HAL.c�и����������ܶ���
  *ʵ���Һ������ȶ�û�иĶ����Ǳ��ļ����Բ���������ֱ��ʹ�ã��ͻ���Ӧ�ò�ֱ�ӵ�
  *�ñ��ļ������Ϳ���ʵ�ָ��ֲ�����
*******************************************************************************/

#include "M90_M91_Interface.h"
#include <string.h>
#include "usart.h"	  
#include "TIM.h"	  
#include "Lora.h"	  
#include "time.h"	  

char *M91_Vel = NULL;    
LoRaNode_Info LoRaNode;
LoRaNode_Info *LoRaNode_str = &LoRaNode;

Status_Info LoRaNode_Status;
Status_Info *LoRaNode_Status_str = &LoRaNode_Status;

#define RXLEN   usart3RxBufferSize
#define AT_Data_buf usart3RxBuffer

//--------------------------------M90״̬���ƺ�����-------------------------------//
/**
  * ��飺�ú�������M90ģ����г�ʼ����              
  * ������ ��  
  * ����ֵ����
  */
void M90_M91_Init(void)
{
    LoRaNode_GPIO_Init();   
    LoRaNode_UART_Init();
		LoraRxCnt=0;
    LoRaNode_Reset();
    LoRaNode_SetWake(Mode_WakeUp);   
    LoRaNode_SetMode(Mode_CMD);	
    Delay_ms(20);    
}

/**
  * ��飺�ú��������л�M90ģ�鹤��ģʽ��              
  * ������ Mode = Mode_CMD����ģʽ��Mode = Mode_Transparent͸��ģʽ  
  * ����ֵ����
  */
void LoRaNode_SetMode(LoRaNode_Mode_T Mode)
{
    if (Mode == Mode_CMD)
        LoRaNode_MODE_HIGH();
    if (Mode == Mode_Transparent)
        LoRaNode_MODE_LOW();
}

/**
  * ��飺�ú��������л�M90ģ��״̬��              
  * ������ Mode = Mode_WakeUp����״̬��Mode = Mode_Sleep����״̬  
  * ����ֵ����
  */
void LoRaNode_SetWake(LoRaNode_SleepMode_T Mode)
{
    if (Mode == Mode_WakeUp)
    {
//        if(HAL_GPIO_ReadPin(LoRaNode_WAKE_GPIO_PORT,LoRaNode_WAKE_PIN) != GPIO_PIN_SET)
        {
            LoRaNode_WAKE_HIGH();            
            Delay_ms(100);                
        }
    }
    
    if (Mode == Mode_Sleep)
    {
//        if(HAL_GPIO_ReadPin(LoRaNode_WAKE_GPIO_PORT,LoRaNode_WAKE_PIN) != GPIO_PIN_RESET)
        {
            LoRaNode_WAKE_LOW();
            Delay_ms(100);    
        }
    }
}

/**
  * ��飺�ú������ڸ�λM90ģ�顣              
  * ������ ��  
  * ����ֵ����
  */
void LoRaNode_Reset(void)
{
    LoRaNode_NRST_LOW();        
    Delay_ms(50);    //15ms    
    LoRaNode_NRST_HIGH();        
    Delay_ms(50);    //15ms
}

//--------------------------------ATָ�����-------------------------------//
/**
  * ��飺�ú������ڶ�ȡM90�̼��汾��              
  * ������ ��  
  * ����ֵ���̼��汾
  */
char *LoRaNode_GetVer(void)
{
    uint8_t ASK_Ver[] = "AT+Ver?\r\n";
    char *temp = "+VER:";
    memset(AT_Data_buf,0,RXLEN);              
		LoraRxCnt=0;
    LoRaNode_Send_AT(ASK_Ver);    
    Delay_ms(50); 
    LoRaNode_Read(2);        
    if(StringStr((char *)AT_Data_buf, temp) != NULL)
    {
        M91_Vel = StringStr((char *)AT_Data_buf, temp);
        return M91_Vel;
    }    
    return M91_Vel;
}
/**
  * ��飺�ú�������ͨ��ATָ���ȡM90��������������ֵ����ͨ��ָ����ʽ���أ���              
  * ������ AT_Command ATָ�  *AT_Value M90����ֵ
  * ����ֵ��AT_Value
  */
void LoRaNode_Getpoint(uint8_t *AT_Command,uint8_t *AT_Value)
{
    uint8_t Command[20] = {0};
    uint8_t Check[10] = {0};
    char *ptr = NULL;   
    uint8_t stringlen;
    uint8_t len = 0;
    
    stringlen = strlen((const char*)AT_Command);
    strcpy((char*)Command,(const char*)AT_Command);   
    Command[stringlen++] = '\r';
    Command[stringlen++] = '\n';
    Command[stringlen] = '\0';
    
    memset(AT_Data_buf,0,RXLEN);               
		LoraRxCnt=0;
    LoRaNode_Send_AT(Command);    
    LoRaNode_Read(2);
    strncpy((char*)Check,(const char*)AT_Command+2,stringlen-5);
    if((ptr = StringStr((char *)AT_Data_buf, (char*)Check)) != NULL)
    {
        if(!(strcmp((const char*)Check,"+DEVEUI")&&strcmp((const char*)Check,"+APPEUI")))
        {
            len = 8;
        }
        if(!strcmp((const char*)Check,"+DEVADDR"))
        {
            len = 4;
        }
        if(!(strcmp((const char*)Check,"+APPKEY")&&strcmp((const char*)Check,"+APPSKEY")\
            &&strcmp((const char*)Check,"++NWKSKEY")))
        {
            len = 16;
        }
        if(len > 0)
        {
            for(uint8_t i=0;i<len;i++)
            {
                uint8_t temp1=0,temp2=0;
                
                temp1 = *((ptr + stringlen - 4+(3*i))+1);
                temp2 = *((ptr + stringlen - 4+(3*i))+2);
                if(temp1 > 0x40)
                {
                    temp1 = temp1 - 55;
                }else{
                    temp1 = temp1 - 48;
                }
                if(temp2 > 0x40)
                {
                    temp2 = temp2 - 55;
                }else{
                    temp2 = temp2 - 48;
                }
                AT_Value[i] = temp1*16 + temp2;
            }
        }
        else
        {
            AT_Value[0]  = (*(ptr + stringlen - 3)) - 0x30; 
        }
    }   
}

/**
  * ��飺�ú�������ͨ��ATָ���ȡM90״ֵ̬��              
  * ������ *LoRa_temp ��ŷ��ص�״ֵ̬
  * ����ֵ��LoRa_temp
  */
void LoRaNode_GetState(Status_Info *LoRa_temp)                                    
{
    uint8_t GetSTATUS[] = "AT+STATUS?\r\n\0";
    char *temp = "+STATUS:";
    char *ptr = NULL;    
    uint8_t dec=0,dec1=0,dec2=0,dec3=0,dec4=0,dec5=0,dec6=0,dec7=0;
    

    LoRaNode_SetWake(Mode_WakeUp);
    HAL_Delay(20);
    // ��������ģʽ
    LoRaNode_SetMode(Mode_CMD);
    HAL_Delay(20);

		memset(AT_Data_buf,0,RXLEN);            
		LoraRxCnt=0;
    LoRaNode_Send_AT(GetSTATUS);       
    LoRaNode_Read(2);        
    if((ptr = StringStr((char *)AT_Data_buf, temp)) != NULL)
    {
        dec =StrToHex( *(ptr + 9));
        dec1 = StrToHex( *(ptr + 10));
        
        LoRa_temp->Up_Result = dec*16 + dec1;
        
        dec = StrToHex( *(ptr + 12));
        dec1 = StrToHex( *(ptr + 13));
        
        LoRa_temp->Up_CH = dec*16 + dec1;
        
        dec = StrToHex( *(ptr + 15));
        dec1 = StrToHex( *(ptr + 16));
        
        LoRa_temp->Up_Rate = dec*16 + dec1;
        
        dec = StrToHex( *(ptr + 18));
        dec1 = StrToHex( *(ptr + 19));
        
        LoRa_temp->Up_TxPower = dec*16 + dec1;
        
        dec = StrToHex( *(ptr + 66));
        dec1 = StrToHex( *(ptr + 67));
        dec2 = StrToHex( *(ptr + 69));
        dec3 = StrToHex( *(ptr + 70));
        
        LoRa_temp->Up_Cache = (dec*16+ dec1) +(dec2*16 + dec3)*256 ;
        
        dec = StrToHex( *(ptr + 72));
        dec1 = StrToHex( *(ptr + 73));
        dec2 = StrToHex( *(ptr + 75));
        dec3 = StrToHex( *(ptr + 76));
        
        LoRa_temp->Up_nbTrials = (dec*16+ dec1) +(dec2*16 + dec3)*256 ;
        
        dec = StrToHex( *(ptr + 117));
        dec1 = StrToHex( *(ptr + 118));
        dec2 = StrToHex( *(ptr + 120));
        dec3 = StrToHex( *(ptr + 121));
        dec4 = StrToHex( *(ptr + 123));
        dec5 = StrToHex( *(ptr + 124));
        dec6 = StrToHex( *(ptr + 126));
        dec7 = StrToHex( *(ptr + 127));
        
        LoRa_temp->Up_Cnt = (dec*16+ dec1) +(dec2*16 + dec3)*256 + (dec4*16+ dec5)*65536 +(dec6*16 + dec7)*256*65536 ;
        
        dec = StrToHex( *(ptr + 21));
        dec1 = StrToHex( *(ptr + 22));
        LoRa_temp->Down_CH = dec*16 + dec1;
        
        dec = StrToHex( *(ptr + 24));
        dec1 = StrToHex( *(ptr + 25));
        LoRa_temp->Down_Rate = dec*16 + dec1;                
        
        dec = StrToHex( *(ptr + 30));
        dec1 = StrToHex( *(ptr + 31));
        LoRa_temp->Down_RxSlot = dec*16 + dec1;                   
        
        dec = StrToHex( *(ptr + 33));
        dec1 = StrToHex( *(ptr + 34));
        LoRa_temp->Down_Port = dec*16 + dec1;                   
        
        dec = StrToHex( *(ptr + 42));
        dec1 = StrToHex( *(ptr + 43));
        LoRa_temp->Down_SNR = dec*16 + dec1;                   
        
        dec = StrToHex( *(ptr + 48));
        dec1 = StrToHex( *(ptr + 49));
        dec2 = StrToHex( *(ptr + 51));
        dec3 = StrToHex( *(ptr + 52));
        LoRa_temp->Down_RSSI = (dec*16 + dec1) + (dec2*16 + dec3) * 256;                   
        
        dec = StrToHex( *(ptr + 54));
        dec1 = StrToHex( *(ptr + 55));
        dec2 = StrToHex( *(ptr + 57));
        dec3 = StrToHex( *(ptr + 58));
        dec4 = StrToHex( *(ptr + 60));
        dec5 = StrToHex( *(ptr + 61));
        dec6 = StrToHex( *(ptr + 63));
        dec7 = StrToHex( *(ptr + 64));
        LoRa_temp->Down_Cnt = (dec*16+ dec1) +(dec2*16 + dec3)*256 + (dec4*16+ dec5)*65536 +(dec6*16 + dec7)*256*65536 ;
        
    }  
    LoRaNode_SetMode(Mode_Transparent);
    HAL_Delay(20);
    LoRaNode_SetWake(Mode_Sleep);
    HAL_Delay(20);
}
/**
  * ��飺�ú�������ͨ��ATָ������M90��GPIO״̬��              
  * ������ pin GPIO�ڣ� state �ߵ͵�ƽ״̬
  * ����ֵ��0������ȷ��-1���ô���
  */
int LoRaNode_SetGPIO(uint32_t pin, uint32_t state)
{    
    uint8_t GPIO[20] = "AT+GPIO=";
    uint8_t buf[5] = {0};
    char *temp = "OK";
    char *ptr = (char*)GPIO;
    
    IntToStr(buf, pin);
    strcat((char *)GPIO, (char *)buf);    
    while(*++ptr);    
    *ptr++ = ',';    
    memset(buf,0,5);
		LoraRxCnt=0;
    IntToStr(buf, state);
    StringConcat(GPIO, buf);    
    memset(AT_Data_buf,0,RXLEN);           
		LoraRxCnt=0;
    LoRaNode_Send_AT(GPIO);    
    LoRaNode_Read(2);                
    if((ptr = StringStr((char *)AT_Data_buf, temp)) != NULL)
    {
        return 0;
    }    
    return -1;
}
/**
  * ��飺�ú�������ͨ��ATָ������M90�Ĳ����������ڲ���Ϊ���ͣ���              
  * ������ AT_Command ATָ�AT_Value ����ֵ
  * ����ֵ��0������ȷ��-1���ô���
  */
int LoRaNode_Setinteger(char *AT_Command,uint32_t AT_Value)
{    
    uint8_t Command[20] = {0};
    uint8_t buf[10] = {0};
    char *temp = "OK";
    
    strcpy((char*)Command,(const char*)AT_Command);   
    IntToStr(buf, AT_Value);
    StringConcat(Command, buf);    
    memset(AT_Data_buf,0,RXLEN);
		LoraRxCnt=0;
    LoRaNode_Send_AT(Command);    
    LoRaNode_Read(2);                
    Delay_ms(100);   
    if(StringStr((char *)AT_Data_buf, temp) != NULL)
    {
        return 0;
    }    
    return -1;
}

/**
  * ��飺�ú�������ͨ��ATָ������M90�Ĳ����������ڲ���Ϊ���飬�ַ�������              
  * ������ AT_Command ATָ�AT_Key ����ֵ
  * ����ֵ��0������ȷ��-1���ô���
  */
int LoRaNode_Setpoint(char *AT_Command,uint8_t *AT_Key)
{
    uint8_t Command[80] = {0};
    char *temp = "OK";
    
    strcpy((char*)Command,(const char*)AT_Command);
    StringConcat(Command, AT_Key);    
    memset(AT_Data_buf,0,RXLEN);               
		LoraRxCnt=0;
    LoRaNode_Send_AT(Command);
    LoRaNode_Read(2);                    
    Delay_ms(100);     
    if(StringStr((char *)AT_Data_buf, temp) != NULL)
    {
        return 0;
    }   
    return -1;    
}

/**
  * ��飺�ú�������ͨ��ATָ������M90��Ƶ�㡣              
  * ������ Up_Dn ���л������У�Ch_Cnt �ŵ�������Start_Freq ��ʼƵ��
  * ����ֵ��0������ȷ��-1���ô���
  */
int LoRaNode_SetFreq(uint8_t Up_Dn,uint8_t Ch_Cnt,uint32_t Start_Freq)
{
    char FREQ[30];
    char *temp = "OK";
    
    sprintf(FREQ,"AT+FREQ=%d,%d,%d\r\n",Up_Dn,Ch_Cnt,Start_Freq);    
    memset(AT_Data_buf,0,RXLEN);               
		LoraRxCnt=0;
    LoRaNode_Send_AT((uint8_t *)FREQ);    
    LoRaNode_Read(2);                    
    if(StringStr((char *)AT_Data_buf, temp) != NULL)
    {
        return 0;
    }    
    return -1;
}

/**
  * ��飺�ú���������M90������Mini RFģʽ�£�ͨ��ATָ������Mini RF����Ƶ������              
  * ������ ���˵���ֲ�
  * ����ֵ��0������ȷ��-1���ô���
  */
int LoRaNode_SetP2P(uint32_t f,uint8_t a,uint8_t b,uint8_t c,uint8_t d,uint8_t e,uint8_t ff,uint8_t g,uint16_t h)
{
    uint8_t SetDebug[50] = "AT+RADIO=";
    uint8_t buf[10] = {0}; 
    uint8_t buf1[10] = {0}; 
    uint8_t buf2[10] = {0}; 
    uint8_t buf3[10] = {0}; 
    uint8_t buf4[10] = {0}; 
    uint8_t buf5[10] = {0}; 
    uint8_t buf6[10] = {0}; 
    uint8_t buf7[10] = {0}; 
    uint8_t buf8[10] = {0}; 
    
    uint8_t dou[2] = ",";
    char *temp = "OK";
    
    IntToStr(buf, f);
    StringConcat2(SetDebug, buf);
    StringConcat2(SetDebug, dou);
    
    IntToStr(buf1, a);
    StringConcat2(SetDebug, buf1);
    StringConcat2(SetDebug, dou);
    
    IntToStr(buf2, b);
    StringConcat2(SetDebug, buf2);
    StringConcat2(SetDebug, dou);
    
    IntToStr(buf3, c);
    StringConcat2(SetDebug, buf3);
    StringConcat2(SetDebug, dou);
    
    IntToStr(buf4, d);
    StringConcat2(SetDebug, buf4);
    StringConcat2(SetDebug, dou);
    
    IntToStr(buf5, e);
    StringConcat2(SetDebug, buf5);
    StringConcat2(SetDebug, dou);
    
    IntToStr(buf6, ff);
    StringConcat2(SetDebug, buf6);
    StringConcat2(SetDebug, dou);
    
    IntToStr(buf7, g);
    StringConcat2(SetDebug, buf7);
    StringConcat2(SetDebug, dou);
    
    IntToStr(buf8, h);
    StringConcat(SetDebug, buf8);
    
    memset(AT_Data_buf,0,RXLEN);           
		LoraRxCnt=0;
    
    LoRaNode_Send_AT(SetDebug);
    Delay_ms(50);
    LoRaNode_Read(2);                
    
    if(StringStr((char *)AT_Data_buf, temp) != NULL)
    {
        return 0;
    }
    
    return -1;
}

/**
  * ��飺�ú�������ͨ��ATָ��ͬ��ʵʱʱ�ӣ�Ҫ��join�����ʹ�á�              
  * ������ 
  * ����ֵ��
  */
int LoRaNode_SyncRTC(void)
{
    uint8_t GetRTC[] = "AT+TIMESYNC\r\n\0";
    char *temp = "OK";
    char *temp1 = "+RTC INFO:";
    char *ptr = NULL; 
		struct tm gm_date;
		uint32_t LoRa_rtc;
		char tmpstr[100];
	
    LoRaNode_SetWake(Mode_WakeUp);
    HAL_Delay(20);
    // ��������ģʽ
    LoRaNode_SetMode(Mode_CMD);
    HAL_Delay(20);

		memset(AT_Data_buf,0,RXLEN);            
		LoraRxCnt=0;
    LoRaNode_Send_AT(GetRTC);       
    LoRaNode_Read(5);                    
		LoRaNode_Read(10); 
    HAL_Delay(6000);
    if(StringStr((char *)AT_Data_buf, temp) != NULL)
    {
				if(StringStr((char *)AT_Data_buf, temp1) != NULL)
				{
						ptr=StringStr((char *)AT_Data_buf, temp1);
						ptr+=11;
						gm_date.tm_year=*ptr-'0';
						gm_date.tm_year*=10;
						ptr++;
						gm_date.tm_year+=*ptr-'0';
						ptr+=2;
						gm_date.tm_year=gm_date.tm_year+2000-1900;
					
						gm_date.tm_mon=*ptr-'0';
						gm_date.tm_mon*=10;
						ptr++;
						gm_date.tm_mon+=*ptr-'0';
						ptr+=2;
						gm_date.tm_mon--;
					
						gm_date.tm_mday=*ptr-'0';
						gm_date.tm_mday*=10;
						ptr++;
						gm_date.tm_mday+=*ptr-'0';
						ptr+=2;

						gm_date.tm_hour=*ptr-'0';
						gm_date.tm_hour*=10;
						ptr++;
						gm_date.tm_hour+=*ptr-'0';
						ptr+=2;

						gm_date.tm_min=*ptr-'0';
						gm_date.tm_min*=10;
						ptr++;
						gm_date.tm_min+=*ptr-'0';
						ptr+=2;

						gm_date.tm_sec=*ptr-'0';
						gm_date.tm_sec*=10;
						ptr++;
						gm_date.tm_sec+=*ptr-'0';

						LoRa_rtc=mktime(&gm_date);
						gm_date = *localtime((time_t*)(&LoRa_rtc));
						
						sprintf(tmpstr,"��ȡ��ʱ�䣺%4d��%2d��%2d�� %2dʱ%2d��%2d��\r\n",gm_date.tm_year+1900,gm_date.tm_mon+1,gm_date.tm_mday,gm_date.tm_hour,gm_date.tm_min,gm_date.tm_sec);
						DEBUG_SendStr(tmpstr);
						SetSystime(LoRa_rtc-28800);//�õ�����ʱ��
						ClrLoraBuffer();
						return 0;
				}
				DEBUG_SendStr("��ȡʱ�����2��\r\n");
				return -2;
    }    
		DEBUG_SendStr("��ȡʱ�����1��\r\n");
    return -1;
}
/**
  * ��飺�ú�������ͨ��ATָ���ȡM90״ֵ̬��              
  * ������ *LoRa_temp ��ŷ��ص�״ֵ̬
  * ����ֵ��LoRa_temp
  */
void LoRaNode_GetRTC(uint32_t *LoRa_rtc)                                    
{
		struct tm gm_date;

    uint8_t GetRTC[] = "AT+RTC?\r\n\0";
    char *temp = "+RTC:";
    char *ptr = NULL; 
		char tmpstr[100];

    LoRaNode_SetWake(Mode_WakeUp);
    HAL_Delay(20);
    // ��������ģʽ
    LoRaNode_SetMode(Mode_CMD);
    HAL_Delay(20);

		memset(AT_Data_buf,0,RXLEN);            
		LoraRxCnt=0;
    LoRaNode_Send_AT(GetRTC);       
    LoRaNode_Read(2);        
    if((ptr = StringStr((char *)AT_Data_buf, temp)) != NULL)
    {
				ptr+=6;
				gm_date.tm_year=*ptr-'0';
				gm_date.tm_year*=10;
				ptr++;
				gm_date.tm_year+=*ptr-'0';
				ptr+=2;
				gm_date.tm_year=gm_date.tm_year+2000-1900;
			
				gm_date.tm_mon=*ptr-'0';
				gm_date.tm_mon*=10;
				ptr++;
				gm_date.tm_mon+=*ptr-'0';
				ptr+=2;
				gm_date.tm_mon--;
			
				gm_date.tm_mday=*ptr-'0';
				gm_date.tm_mday*=10;
				ptr++;
				gm_date.tm_mday+=*ptr-'0';
				ptr+=2;

				gm_date.tm_hour=*ptr-'0';
				gm_date.tm_hour*=10;
				ptr++;
				gm_date.tm_hour+=*ptr-'0';
				ptr+=2;

				gm_date.tm_min=*ptr-'0';
				gm_date.tm_min*=10;
				ptr++;
				gm_date.tm_min+=*ptr-'0';
				ptr+=2;

				gm_date.tm_sec=*ptr-'0';
				gm_date.tm_sec*=10;
				ptr++;
				gm_date.tm_sec+=*ptr-'0';

				*LoRa_rtc=mktime(&gm_date);
				gm_date = *localtime((time_t*)(LoRa_rtc));
				
				sprintf(tmpstr,"��ȡ��ʱ�䣺%4d��%2d��%2d�� %2dʱ%2d��%2d��\r\n",gm_date.tm_year+1900,gm_date.tm_mon+1,gm_date.tm_mday,gm_date.tm_hour,gm_date.tm_min,gm_date.tm_sec);
				DEBUG_SendStr(tmpstr);
    }  
    LoRaNode_SetMode(Mode_Transparent);
    HAL_Delay(20);
    LoRaNode_SetWake(Mode_Sleep);
    HAL_Delay(20);
}


//--------------------------------����ͨ�ź�����-------------------------------//
/**
  * ��飺ͨ��������M90ģ�鷢�����ݡ�              
  * ������ pdata Ҫ���͵����ݣ� Length ����  
  * ����ֵ����
  */
void LoRaNode_SendData(uint8_t *pdata, uint16_t Length)  
{  
    uint32_t i = 0;
    
    for (i = 0; i < Length; i++)
    {
        LoRaNode_UART_Send_Byte(pdata[i]);
    }
}

/**
  * ��飺ͨ��������M90ģ�鷢��ATָ�              
  * ������ at_buf Ҫ���͵�ATָ��  
  * ����ֵ����
  */
void LoRaNode_Send_AT(uint8_t *at_buf)
{
    LoRaNode_UART_Send_String((char*)at_buf);
}

/**
  * ��飺ͨ�����ڶ�ȡM90ģ�鷢�ص����ݡ�              
  * ������ str ���ص�����  
  * ����ֵ����
  */
void LoRaNode_Read(uint8_t S)
{
	TIM7_SingleStart_s(S);
//  memset(AT_Data_buf,0,RXLEN);               
//	usart3RxCnt=0;
	while((LoRaNode_UART_receive_flag != 1)&&(SysTickSCount_flag != 1))
	{        
	}
	TIM_Cmd(TIM7, DISABLE);//�ر��ж�

	LoRaNode_UART_receive_flag = 0;
	LoRaNode_ReadTimeout_flag = 0;
}


