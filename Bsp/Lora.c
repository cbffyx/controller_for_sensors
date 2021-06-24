#include "Lora.h"

char *strx,*extstrx;
extern package send_package;

void Lora_GPIO_Init(void)
{
    M90_M91_Init();
}

// ���û��޸ģ�
// ����ģ�����
int LoRaWAN_Node_SetParameter(void)
{
    char *strx = NULL;
    char *strx1 = NULL;
    int result = 0;
    uint8_t i;
    char tmpstr[50];
    // ����ģ��
    LoRaNode_SetWake(Mode_WakeUp);
    HAL_Delay(10);
    // ģ���������ģʽ
    LoRaNode_SetMode(Mode_CMD);
    HAL_Delay(20);

    ClrLoraBuffer();
    LoRaNode_UART_receive_flag=0;
    LoRaNode_Setpoint("AT+DEVEUI?",(uint8_t *)"\0");
    Delay_ms(500);
    strx1=strstr((const char*)&LoraRxBuffer, (const char*)"+DEVEUI:");//+DEVEUI: 00 95 69 00 66 00 00
    strx = strx1+9;
    if(NULL != strx1)
    {
        memset(LoRaNode.DevEUI,0,20);
        for (i=0; i<16; i++)
        {
            //memcpy(&LoRaNode.DevEUI[i],strx,2);
						LoRaNode.DevEUI[i]=StrToHex(*(strx));strx++;
						LoRaNode.DevEUI[i]=(LoRaNode.DevEUI[i]<<4)+StrToHex(*(strx));strx++;
            strx++;
        }
        sprintf(tmpstr,"LoraID��ȡ�ɹ�:%02X%02X%02X%02X%02X%02X%02X%02X\r\n",LoRaNode.DevEUI[0],LoRaNode.DevEUI[1],LoRaNode.DevEUI[2],LoRaNode.DevEUI[3],LoRaNode.DevEUI[4],LoRaNode.DevEUI[5],LoRaNode.DevEUI[6],LoRaNode.DevEUI[7]);
        DEBUG_SendStr(tmpstr);
    }
		else
		{
				DEBUG_SendStr("LoraID��ȡʧ�ܣ�\r\n");
				LoRaNode_Setpoint("AT+DEVEUI?",(uint8_t *)"\0");
				Delay_ms(500);
				strx1=strstr((const char*)&LoraRxBuffer, (const char*)"+DEVEUI:");//+DEVEUI: 00 95 69 00 66 00 00
				strx = strx1+9;
				if(NULL != strx1)
				{
						memset(LoRaNode.DevEUI,0,20);
						for (i=0; i<16; i++)
						{
								//memcpy(&LoRaNode.DevEUI[i],strx,2);
								LoRaNode.DevEUI[i]=StrToHex(*(strx));strx++;
								LoRaNode.DevEUI[i]=(LoRaNode.DevEUI[i]<<4)+StrToHex(*(strx));strx++;
								strx++;
						}
						sprintf(tmpstr,"LoraID��ȡ�ɹ�:%02X%02X%02X%02X%02X%02X%02X%02X\r\n",LoRaNode.DevEUI[0],LoRaNode.DevEUI[1],LoRaNode.DevEUI[2],LoRaNode.DevEUI[3],LoRaNode.DevEUI[4],LoRaNode.DevEUI[5],LoRaNode.DevEUI[6],LoRaNode.DevEUI[7]);
						DEBUG_SendStr(tmpstr);
				}
				else
				{
					DEBUG_SendStr("LoraID��ȡʧ�ܣ�����\r\n");
				}
		}

    LoRaNode.Band=7;
    LoRaNode.ADR=0;
    //LoRaNode.Confirm=0;
    LoRaNode.Confirm=1;
    LoRaNode.Power=0;
    LoRaNode.NET_Mode=NET_OTAA;
    LoRaNode.Class_x=Class_A;

		result += LoRaNode_Setinteger("AT+DEBUG=",0);
		result=0;
    // ����Ƶ��
    result += LoRaNode_Setinteger("AT+BAND=",LoRaNode.Band);//6 - CN470������������Ƶ 7 - CN470����������ͬƵ
    result += LoRaNode_Setinteger("AT+JOIN=",0);//1�C ���JOIN�����ģ��û�����ģ���ģ�鿪��һ���µ�JOIN���̡�
    result += LoRaNode_Setinteger("AT+RX1=",2000);//����ʱ���Ϊ2s
    result += LoRaNode_Setinteger("AT+NBTRIALS=",3);//���ʹ���
    result += LoRaNode_Setinteger("AT+ADR=",LoRaNode.ADR);
    // 0->20dBm, 1->17, 2->16dBm, 3->14dBm, 4->12dBm, 5->10dBm, 6->7dBm, 7->5dBm, 8->2dBm
    result += LoRaNode_Setinteger("AT+Power=", LoRaNode.Power);
    // 0->SF12, 1->SF11, 2->SF10, 3->SF9, 4->SF8, 5->SF7
    result += LoRaNode_Setinteger("AT+DATARATE=", 3);//SF10,11,12:51byte SF9:115byte   SF7,8:222byte	
    // 0:UNCONFIRM 1:CONFIRM
    result += LoRaNode_Setinteger("AT+CONFIRM=", LoRaNode.Confirm);
    // ����RX2
    result += LoRaNode_Setpoint("AT+RX2=",(uint8_t *)"3,475300000");//5 - DR5 (SF7 BW125)    3->SF9
    // ����CLASS
    result += LoRaNode_Setinteger("AT+CLASS=",LoRaNode.Class_x);
    // ����OTAA
    result += LoRaNode_Setinteger("AT+CSMA=",0);//AT+CSMA=X1
    // ����CSMA����˱ܹر�   ����
    result += LoRaNode_Setinteger("AT+OTAA=",LoRaNode.NET_Mode);
    // �޸�һ��Ϊ����Ӧ�õ�APPEUI
//  result += LoRaNode_Setpoint("AT+APPEUI=",(uint8_t *)Lora_APPEUI);
    result += LoRaNode_Setpoint("AT+APPEUI=",(uint8_t *)LoRaNode.AppEUI);
    Delay_ms(30);
    // �޸�һ��Ϊ����Ӧ�õ�APPKEY
//	result += LoRaNode_Setpoint("AT+APPKEY=",(uint8_t *)Lora_APPKEY);
    result += LoRaNode_Setpoint("AT+APPKEY=",(uint8_t *)LoRaNode.AppKEY);
    Delay_ms(30);

    result += LoRaNode_Setpoint("AT+SAVE",(uint8_t *)"\0");
    Delay_ms(200);

    return result;
}

void LoRaWAN_Join(void) {
	  extern uint8_t TimeOutSign;
    // ����ģ��
    LoRaNode.Join=0;
    LoRaNode_Reset();
    Delay_ms(500);
    // ����͸��ģʽ
    LoRaNode_SetMode(Mode_Transparent);
    DEBUG_SendStr("Lora����join...");
    // �ж�������־
    while((LoRaNode_STAT_STATUS() == RESET)||(LoRaNode_BUSY_STATUS() == RESET))  //BKP_TamperPinCmd(DISABLE);
    {
        DEBUG_SendStr(".");
        Delay_ms(1000);
    }
    DEBUG_SendStr("\n");
    DEBUG_SendStr("Lora�����ɹ�\n");
    LoRaNode.Join=1;
    LoRaNode_UART_receive_flag=0;
    ClrLoraBuffer();
		TimeOutSign=0;
		while ((LoRaNode_UART_receive_flag==0)&&Time_Out_Break(300,&TimeOutSign)==0);//��ʱ5��
}

int LoRaWAN_Node_Send(uint8_t *buf, uint16_t len, uint8_t confirm)
{
    // ����ģ��
    LoRaNode_SetWake(Mode_WakeUp);
    HAL_Delay(20);
    // ����͸��ģʽ
    LoRaNode_SetMode(Mode_Transparent);
    HAL_Delay(20);

    uint8_t TimeOut_Sign = 0;
    // �ȴ�ģ�����
    while(LoRaNode_BUSY_STATUS() == RESET)
    {
        if(Time_Out_Break(20000, &TimeOut_Sign) == 1)
        {
            return -1;
        }
    }
    LoRaNode_UART_receive_flag=0;
    ClrLoraBuffer();
    LoRaNode_UART_Send_SendData((char *)buf,len);
    //LoRaNode_Send_AT(buf);

    //----�ȴ� BUSY ���Ŵ���æ״̬(�͵�ƽ)���ȴ�ʱ���Լӳ�ʱ�ж�
    TimeOut_Sign = 0;
    while(LoRaNode_BUSY_STATUS() != RESET)
    {
        if(Time_Out_Break(2000, &TimeOut_Sign) == 1)
        {
            return -2;  // ���� -2  : ģ��û�н��յ����ڷ��͵����ݣ�����ģ��û�й���
        }
    }

    //----�ȴ����ݷ������  BUSY ���Żص�����״̬(�ߵ�ƽ)���ȴ�ʱ���Լӳ�ʱ�ж�
    TimeOut_Sign = 0;
    while(LoRaNode_BUSY_STATUS() == RESET)
    {
        if(Time_Out_Break(60000, &TimeOut_Sign) == 1)
        {
            return -3; // ���� -3  : ģ�鹤���쳣
        }
    }

    if(confirm == 1)
    {
        if(LoRaNode_STAT_STATUS() == RESET)
        {
            return -4; // ��Ϊȷ��֡�����ʾģ��δ�յ�ACK
        }
				else
				{
					send_package.serial++;
				}
    }

    //HAL_Delay(500);

    LoRaNode_SetMode(Mode_Transparent);
    //LoRaNode_MODE_LOW();

    return 0; // ����ͨ�ųɹ�
}

// ��ʱ�ж�
uint8_t Time_Out_Break(uint32_t MAX_time , uint8_t *Sign)
{
    static uint32_t time_start = 0;
    static uint32_t time_new = 0;
    uint32_t temp=0;
    uint8_t TimeOut_Sign = *Sign;

    if(TimeOut_Sign == 0)
    {
        *Sign = 1;
        time_start = HAL_GetTick();
    }
    if(TimeOut_Sign == 1)
    {
        time_new = HAL_GetTick();

        if(time_new < time_start)
        {
            time_new = (time_new + (0xffffffff - time_start));
            time_start = 0;
        }
        temp = time_new - time_start;
        if(temp > MAX_time)
        {
            return 1;
        } else {
            return 0;
        }
    }
    return 0;
}

int SyncGetTime(void)
{
//		uint32_t LoraTime;
		if (LoRaNode_SyncRTC()==0)
		{
//			LoRaNode_GetRTC(&LoraTime);
//			SetSystime(LoraTime);
			return 0;
		}
		return 1;
}

int Lora_Init(void)
{
    int state = 0;

    LoRaNode_SetMode(Mode_CMD);	
    M90_M91_Init();
    Read_APPEUI_APPKEY((char *)LoRaNode.AppEUI,(char *)LoRaNode.AppKEY);	//ReadOut LoRaNode.DevEUI and LoRaNode.APPKEY from Flash
    state = LoRaWAN_Node_SetParameter();//Read LoRaNode.DevEUI
    if(state < 0)
    {
        DEBUG_SendStr("Lora�������ô���\n");
        return -1;
    } else {
        DEBUG_SendStr("Lora�������óɹ�\n");
    }
		
    // ģ������
    //HAL_Delay(1000);
    LoRaWAN_Join();
		LoRaNode_GetState(&LoRaNode_Status);
		SyncGetTime();
		
    /*
    	// ��������
    state = LoRaWAN_Node_Send(SS_Data,lll?, 1);//�����ַ���
    if(state == 0)
    {
    	DEBUG_Printf("�������гɹ�, ��������:");
    	for(int i = 0; i<5; i++)
    	{
    		DEBUG_Printf("%02x ", SS_Data[i]);
    	}
    	DEBUG_Printf("\n");
    }
    else
    {
    	DEBUG_Printf("�������д���,�������:%d\n", state);
    }
    */
    return 0;
}

