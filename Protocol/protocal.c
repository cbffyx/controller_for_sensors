/**
  * @brief  通信协议 program.
  * @description 按照数据格式定义的协议
  * @param  teper
  */

#include "protocal.h"
#include "STM32L151_Flash.h"
#include "iap.h"
#include <string.h>

unsigned char ProtacalRcvBuffer[LoraRxBufferSize/2];

void setState(enum sys_state state);
extern uint8_t GetRequestUpdateOK;
extern uint16_t wakeup_times;
extern uint16_t Dfrequency;
extern uint8_t TimeOutSign;

extern uint8_t FAC_SN[3];
extern uint8_t ServerIP1[4];//生产服务器IP
extern uint8_t ServerPort1[2];//生产服务器端口
extern uint8_t ServerIP2[4];//正式服务器IP
extern uint8_t ServerPort2[2];//正式服务器端口
package send_package;
volatile sys_time time,last_time;
static enum sys_state new_state;
static char send_buffer[100];
static uint8_t sensorDataSendState;

UpdateFileInfo updateFileInfo;
extern SensorSInfo sensorSInfo;
extern SensorSInfo sensorReal;

extern void SysStateChange(void);
extern void SetDfrequency(uint16_t dat);
volatile uint8_t LoraRetryTimes=0;

extern uint16_t daytime;

void setID(uint8_t* id)
{
    memset(id, 0, 16);
    memcpy(id,FAC_SN,3);
    memcpy(&id[3],IdF,3);
    memcpy(&id[6],&LoRaNode.DevEUI[4],4);
}

uint8_t formatRealData(uint8_t* ip,uint8_t* port,float temp1,float humidity1,float temp2,float humidity2,float temp3,float humidity3,float pressure,uint8_t energy,uint32_t collect_time)
{
    uint8_t i;
    uint16_t tmpFreq;

    i=0;
    memcpy(&send_buffer[i],ip,4);
    i+=4;
    memcpy(&send_buffer[i],port,2);
    i+=2;
    memcpy(&send_buffer[i],send_package.ID,10);
    i+=10;
    send_buffer[i]=send_package.serial;
    i++;
    send_buffer[i]=MSG_TYPE_SENSOR_DATA_EX;
    i++;
    send_buffer[i]=SW_VER_1;
    i++;
    send_buffer[i]=SW_VER_2;
    i++;
    send_buffer[i]=SW_VER_3;
    i++;
    send_buffer[i]=collect_time>>24;
    i++;
    send_buffer[i]=collect_time>>16;
    i++;
    send_buffer[i]=collect_time>>8;
    i++;
    send_buffer[i]=collect_time;
    i++;
    send_buffer[i]=(LoRaNode_Status.Down_RSSI+113)/2;
    i++;
    send_buffer[i]=(temp1*10)/256;
    i++;
    send_buffer[i]=temp1*10;
    i++;
    send_buffer[i]=(humidity1*10)/256;
    i++;
    send_buffer[i]=humidity1*10;
    i++;
    send_buffer[i]=(temp2*10)/256;
    i++;
    send_buffer[i]=temp2*10;
    i++;
    send_buffer[i]=(humidity2*10)/256;
    i++;
    send_buffer[i]=humidity2*10;
    i++;
    send_buffer[i]=(humidity3*10)/256;
    i++;
    send_buffer[i]=humidity3*10;
    i++;
    send_buffer[i]=(temp3*10)/256;
    i++;
    send_buffer[i]=temp3*10;
    i++;
    send_buffer[i]=(pressure*10)/256;
    i++;
    send_buffer[i]=pressure*10;
    i++;
    send_buffer[i]=energy;
    i++;
    if (Dfrequency==0) {
        tmpFreq=wakeup_times/3;
    }
    else {
        tmpFreq=Dfrequency;
    }
    send_buffer[i]=tmpFreq/256;
    i++;
    send_buffer[i]=tmpFreq;
    i++;
    return i;
}

/*******************************************************************************
* @Function Name	:协议变量初始化
* @Description	:  .
* @Input		: None
* @Output	: None
* @Return	: None
*******************************************************************************/
void ProcotalInit(void) {
    setID(send_package.ID);
    send_package.serial=0;
}
/*******************************************************************************
* @Function Name	:发送数据包
* @Description	:  .
* @Input		: 数据结构，发送方向，发送还是回应，消息类型，传感器类型
* @Output	: None
* @Return	: None
*******************************************************************************/
int SendPacket(uint8_t IPType,SensorSInfo par,uint32_t mess_type,uint8_t* ip,uint8_t* port)
{   // ,0,0,2 --- ,0,0,2，type
    uint8_t SendLen=0;
    uint8_t hc1,tc1,hc2,tc2,hc3,tc3,pc;
    BD Data1;
		int SendResult=0;
		char TmpStrSendPacket[50];

		ClrLoraBuffer();
    memset(send_buffer,0,sizeof(send_buffer));
    SendLen=0;
    if(mess_type==MSG_TYPE_SAMPLE_TIME)//请求升级信息
    {
        memcpy(&send_buffer[SendLen],ip,4);
        SendLen+=4;
        memcpy(&send_buffer[SendLen],port,2);
        SendLen+=2;
        memcpy(&send_buffer[SendLen],send_package.ID,10);
        SendLen+=10;
        send_buffer[SendLen]=send_package.serial;
        SendLen++;
        send_buffer[SendLen]=mess_type;
        SendLen++;
        send_buffer[SendLen]=SW_VER_1;
        SendLen++;
        send_buffer[SendLen]=SW_VER_2;
        SendLen++;
        send_buffer[SendLen]=SW_VER_3;
        SendLen++;
    }
    else if(MSG_TYPE_SENSOR_DATA_EX == mess_type)//扩展上报
    {
        SendLen=formatRealData(ip,port,par.temp1,par.humi1,par.temp2,par.humi2,par.temp3,par.humi3,par.press,par.batPercent,par.ts);
    }
    else if(MSG_TYPE_UPDATE_DATA == mess_type)//请求升级数据
    {
        memcpy(&send_buffer[SendLen],ip,4);
        SendLen+=4;
        memcpy(&send_buffer[SendLen],port,2);
        SendLen+=2;
        memcpy(&send_buffer[SendLen],send_package.ID,10);
        SendLen+=10;
        send_buffer[SendLen]=send_package.serial;
        SendLen++;
        send_buffer[SendLen]=mess_type;
        SendLen++;
        memcpy(&send_buffer[SendLen],updateFileInfo.ver,3);
        SendLen+=3;
        send_buffer[SendLen]=(updateFileInfo.reqOffset>>24);
        SendLen++;
        send_buffer[SendLen]=(updateFileInfo.reqOffset>>16);
        SendLen++;
        send_buffer[SendLen]=(updateFileInfo.reqOffset>>8);
        SendLen++;
        send_buffer[SendLen]=updateFileInfo.reqOffset;
        SendLen++;
        send_buffer[SendLen]=0;
        SendLen++;
        send_buffer[SendLen]=0;
        SendLen++;
        send_buffer[SendLen]=(updateFileInfo.reqLen>>8);
        SendLen++;
        send_buffer[SendLen]=updateFileInfo.reqLen;
        SendLen++;
    }
    else if(MSG_TYPE_CALIB_TIME == mess_type)
    {   //标定
        //{“hc”:1,”tc”:1,”ahc”:1,”ihc”:1,”pc”:1,”mc”:1}
        hc1=ReadBiaoDingData(SD_BD_Addr1,&Data1)+1;
        tc1=ReadBiaoDingData(WD_BD_Addr1,&Data1)+1;
        hc2=ReadBiaoDingData(SD_BD_Addr2,&Data1)+1;
        tc2=ReadBiaoDingData(WD_BD_Addr2,&Data1)+1;
        hc3=ReadBiaoDingData(SD_BD_Addr3,&Data1)+1;
        tc3=ReadBiaoDingData(WD_BD_Addr3,&Data1)+1;
        pc=ReadBiaoDingData(QY_BD_Addr,&Data1)+1;

        memcpy(&send_buffer[SendLen],ip,4);
        SendLen+=4;
        memcpy(&send_buffer[SendLen],port,2);
        SendLen+=2;
        memcpy(&send_buffer[SendLen],send_package.ID,10);
        SendLen+=10;
        send_buffer[SendLen]=send_package.serial;
        SendLen++;
        send_buffer[SendLen]=mess_type;
        SendLen++;
        send_buffer[SendLen]=hc1;
        SendLen++;
        send_buffer[SendLen]=tc1;
        SendLen++;
        send_buffer[SendLen]=hc2;
        SendLen++;
        send_buffer[SendLen]=tc2;
        SendLen++;
        send_buffer[SendLen]=hc3;
        SendLen++;
        send_buffer[SendLen]=tc3;
        SendLen++;
        send_buffer[SendLen]=pc;
        SendLen++;
    }
    Uart1_SendHEXData(send_buffer,SendLen);//Display before send
		SendResult=LoRaWAN_Node_Send((uint8_t *)send_buffer,SendLen,1);
    if (SendResult<0)
    {
        //发送未成功
        if (IPType==IsUserIP)
        {
            sprintf(TmpStrSendPacket,"UserIP SendLora Error %d：",SendResult);
            Uart1_SendStr(TmpStrSendPacket);
        }
        else
        {
            sprintf(TmpStrSendPacket,"FactoryIP SendLora Error %d：",SendResult);
            Uart1_SendStr(TmpStrSendPacket);
        }
        //Uart1_SendHEXData(send_buffer,SendLen);
        Uart1_SendStr("\r\n");
        LoraRetryTimes++;
        if (LoraRetryTimes>5)
        {
            LoraRetryTimes=0;
            Lora_Init();
        }
        return 1;
    }
    else
    {
        LoraRetryTimes=0;
        TimeOutSign=0;
        while ((LoRaNode_UART_receive_flag==0)&&Time_Out_Break(LoraTimeOutDefault,&TimeOutSign)==0);//超时5秒
        if (usart3RxCnt>10)
        {   //answered
            if (IPType==IsUserIP)
            {
                Uart1_SendStr("UserIP SendLora：");
            }
            else
            {
                Uart1_SendStr("FactoryIP SendLora：");
            }
            //Uart1_SendHEXData(send_buffer,SendLen);
            Uart1_SendStr("\r\n");
            LoraRcvHandler(IPType);
            Uart1_SendStr("成功上传和接收！\r\n");
            return 0;
        }
        else
        {   //no answer
            if (IPType==IsUserIP)
            {
                Uart1_SendStr("UserIP SendLora：");
            }
            else
            {
                Uart1_SendStr("FactoryIP SendLora：");
            }
            //Uart1_SendHEXData(send_buffer,SendLen);
            Uart1_SendStr("\r\n");
            Uart1_SendStr("上传未接收到数据！\r\n");
            return 1;
        }
    }
}

/*******************************************************************************
* @Function Name	:解析数据格式
* @Description	:  .
* @Input		: 长度，接收buffer
* @Output	: None
* @Return	: None
*******************************************************************************/
void RecvServerPackage(uint16_t len,uint8_t* buffer,uint8_t IPType) {
    char *strx1 = NULL;
    char *bufferstrx = NULL;
    char ver[3] = {0};
    uint32_t offset = 0;
    uint32_t tmpcrc = 0;
		uint8_t tmpver[3];
		uint32_t tmplen=0;
    uint32_t tmpcrc1 = 0xffffffff;
    uint32_t dataLen = 0;
    uint32_t tmpdat;
    float RealData;
    uint32_t crc_table[256] = {0};
		uint8_t tmpip[4];
		uint8_t tmpport[2];
		uint8_t TmpUpdateData[UPDATE_DOWN_REQ_LEN];
		uint8_t MemUpdateData[UPDATE_DOWN_REQ_LEN];

//找到最后一个0d0a
    strx1=(char *)buffer;
    bufferstrx=strx1;

    bufferstrx=strrchr(strx1,'\n');
    if (bufferstrx==NULL)
    {
        bufferstrx=(char *)buffer;
    }
    else
    {
        bufferstrx+=1;
    }
    ResetSensorDataSendState();
    tmpdat=*(bufferstrx+11);
    switch(tmpdat)
    {
    case MSG_TYPE_CALIB_TIME://标定
    {
#ifdef NetBD
        /*设备号10 帧序号1 消息类型1
        			温度1标定类型 温度1标定数值
        			湿度1标定类型 湿度1标定数值
        			温度2标定类型 温度2标定数值
        			湿度2标定类型 湿度2标定数值
        			温度3标定类型 温度3标定数值
        			湿度3标定类型 湿度3标定数值
        			气压标定类型 气压标定数值*/
//				tmpdat=*(bufferstrx+10);
//				if (tmpdat!=send_package.serial) break;
        if (usart3RxCnt<Recv_CALIB_TIME_len) break;
				if (memcmp(bufferstrx,send_package.ID,sizeof(send_package.ID))!=0) break;
        bufferstrx+=12;
        switch (*bufferstrx)
        {
        case 0://不标定
            break;
        case 1://标定
            RealData=*(bufferstrx+1);
            RealData=RealData*256+*(bufferstrx+2);
            RealData=RealData/10;
            if (AddBD(WD_BD_Addr1,sensorReal.temp1,RealData)==0)
            {
                sensorSInfo.temp1=JS(WD_BD_Addr1,sensorReal.temp1);//重新计算温度
            }
            break;
        case 2://清除
            //清除温度标定
            ClearBD(WD_BD_Addr1);
            sensorSInfo.temp1=JS(WD_BD_Addr1,sensorReal.temp1);//重新计算温度
            break;
        default:
            break;
        }
        bufferstrx+=3;
        switch (*bufferstrx)
        {
        case 0://不标定
            break;
        case 1://标定
            RealData=*(bufferstrx+1);
            RealData=RealData*256+*(bufferstrx+2);
            RealData=RealData/10;
            if (AddBD(SD_BD_Addr1,sensorReal.humi1,RealData)==0)
            {
                sensorSInfo.humi1=JS(SD_BD_Addr1,sensorReal.humi1);
            }
            break;
        case 2://清除
            //清除湿度标定
            ClearBD(SD_BD_Addr1);
            sensorSInfo.humi1=JS(SD_BD_Addr1,sensorReal.humi1);
            break;
        default:
            break;
        }
        bufferstrx+=3;
        switch (*bufferstrx)
        {
        case 0://不标定
            break;
        case 1://标定
            RealData=*(bufferstrx+1);
            RealData=RealData*256+*(bufferstrx+2);
            RealData=RealData/10;
            if (AddBD(WD_BD_Addr2,sensorReal.temp2,RealData)==0)
            {
                sensorSInfo.temp2=JS(WD_BD_Addr2,sensorReal.temp2);//重新计算温度
            }
            break;
        case 2://清除
            //清除温度标定
            ClearBD(WD_BD_Addr2);
            sensorSInfo.temp2=JS(WD_BD_Addr2,sensorReal.temp2);//重新计算温度
            break;
        default:
            break;
        }
        bufferstrx+=3;
        switch (*bufferstrx)
        {
        case 0://不标定
            break;
        case 1://标定
            RealData=*(bufferstrx+1);
            RealData=RealData*256+*(bufferstrx+2);
            RealData=RealData/10;
            if (AddBD(SD_BD_Addr2,sensorReal.humi2,RealData)==0)
            {
                sensorSInfo.humi2=JS(SD_BD_Addr2,sensorReal.humi2);
            }
            break;
        case 2://清除
            //清除湿度标定
            ClearBD(SD_BD_Addr2);
            sensorSInfo.humi2=JS(SD_BD_Addr2,sensorReal.humi2);
            break;
        default:
            break;
        }
        bufferstrx+=3;
        switch (*bufferstrx)
        {
        case 0://不标定
            break;
        case 1://标定
            RealData=*(bufferstrx+1);
            RealData=RealData*256+*(bufferstrx+2);
            RealData=RealData/10;
            if (AddBD(WD_BD_Addr3,sensorReal.temp3,RealData)==0)
            {
                sensorSInfo.temp3=JS(WD_BD_Addr3,sensorReal.temp3);//重新计算温度
            }
            break;
        case 2://清除
            //清除温度标定
            ClearBD(WD_BD_Addr3);
            sensorSInfo.temp3=JS(WD_BD_Addr3,sensorReal.temp3);//重新计算温度
            break;
        default:
            break;
        }
        bufferstrx+=3;
        switch (*bufferstrx)
        {
        case 0://不标定
            break;
        case 1://标定
            RealData=*(bufferstrx+1);
            RealData=RealData*256+*(bufferstrx+2);
            RealData=RealData/10;
            if (AddBD(SD_BD_Addr3,sensorReal.humi3,RealData)==0)
            {
                sensorSInfo.humi3=JS(SD_BD_Addr3,sensorReal.humi3);
            }
            break;
        case 2://清除
            //清除湿度标定
            ClearBD(SD_BD_Addr3);
            sensorSInfo.humi3=JS(SD_BD_Addr3,sensorReal.humi3);
            break;
        default:
            break;
        }
        bufferstrx+=3;
        switch (*bufferstrx)
        {
        case 0://不标定
            break;
        case 1://标定
            RealData=*(bufferstrx+1);
            RealData=RealData*256+*(bufferstrx+2);
            RealData=RealData/10;
            if (AddBD(QY_BD_Addr,sensorReal.press,RealData)==0)
            {
                sensorSInfo.press=JS(QY_BD_Addr,sensorReal.press);
            }
            break;
        case 2://清除
            //清除气压标定
            ClearBD(QY_BD_Addr);
            sensorSInfo.press=JS(QY_BD_Addr,sensorReal.press);
            break;
        default:
            break;
        }
        bufferstrx+=3;
        switch (*bufferstrx)
        {
        case 0://不标定
            break;
        case 1://标定
            RealData=*(bufferstrx+1);
            RealData=RealData*256+*(bufferstrx+2);
            RealData=RealData/10;
            if (AddBD(SF_BD_Addr,sensorReal.moisture,RealData)==0)
            {
                sensorSInfo.moisture=JS(SF_BD_Addr,sensorReal.moisture);
            }
            break;
        case 2://清除
            //清除水份标定
            ClearBD(SF_BD_Addr);
            sensorSInfo.moisture=JS(SF_BD_Addr,sensorReal.moisture);
            break;
        default:
            break;
        }
#endif
    }
    break;
    case MSG_TYPE_SAMPLE_TIME://改为请求升级信息
    {
        /*设备号10 帧序号1 消息类型1
        服务器端软件版本号（3字节）
        服务器端软件固件长度（4字节）
        服务器端软件固件全体校验CRC32（4字节）
        服务器端当前时间：4字节*/
        if (usart3RxCnt<Recv_SAMPLE_TIME_len) break;
				if (memcmp(bufferstrx,send_package.ID,sizeof(send_package.ID))!=0) break;
        bufferstrx+=12;
        //if (IPType==IsFactoryIP)			liyu test
        {
						tmpver[0] = *bufferstrx;
						tmpver[1] = *(bufferstrx+1);
						tmpver[2] = *(bufferstrx+2);
						if (memcmp(tmpver,updateFileInfo.ver,3)!=0) updateFileInfo.reqOffset=0;
						memcpy(updateFileInfo.ver,tmpver,3);
            bufferstrx+=3;

            tmplen=*bufferstrx;
            tmplen=(tmplen<<8)+*(bufferstrx+1);
            tmplen=(tmplen<<8)+*(bufferstrx+2);
            tmplen=(tmplen<<8)+*(bufferstrx+3);
						if (tmplen!=updateFileInfo.size) updateFileInfo.reqOffset=0;
						updateFileInfo.size=tmplen;
            bufferstrx+=4;

            tmpcrc=*bufferstrx;
            tmpcrc=(tmpcrc<<8)+*(bufferstrx+1);
            tmpcrc=(tmpcrc<<8)+*(bufferstrx+2);
            tmpcrc=(tmpcrc<<8)+*(bufferstrx+3);
						if (tmpcrc!=updateFileInfo.crc) updateFileInfo.reqOffset=0;
						updateFileInfo.crc=tmpcrc;
            bufferstrx+=4;

            updateFileInfo.utc=*bufferstrx;
            updateFileInfo.utc=(updateFileInfo.utc<<8)+*(bufferstrx+1);
            updateFileInfo.utc=(updateFileInfo.utc<<8)+*(bufferstrx+2);
            updateFileInfo.utc=(updateFileInfo.utc<<8)+*(bufferstrx+3);
            bufferstrx+=4;

        }
        GetRequestUpdateOK=1;
    }
    break;

    case MSG_TYPE_SENSOR_DATA_EX://扩展上报响应
    {
        /*
        	设备号10 帧序号1 消息类型1
        		服务器端软件版本号3
        		服务器端当前时间4
        		设置的上报周期2
        		后台服务器IP4
        		后台服务器PORT2
        		客户服务器IP4
        		客户服务器PORT2
        		设置的频率4
        		生产日期3
        		*/
        if (usart3RxCnt<Recv_SENSOR_DATA_EX_len) break;
//				tmpdat=*(bufferstrx+10);
//				if (tmpdat!=send_package.serial) break;
        bufferstrx+=12;
        if (IPType==IsUserIP)
        {
            bufferstrx+=3;
            tmpdat=*bufferstrx;
            tmpdat=(tmpdat<<8)+*(bufferstrx+1);
            tmpdat=(tmpdat<<8)+*(bufferstrx+2);
            tmpdat=(tmpdat<<8)+*(bufferstrx+3);
            bufferstrx+=4;
            if (tmpdat!=0) time.time_stamp=tmpdat;
						bufferstrx-=7;
        }
//        else					liyu test
        {
            if (memcmp(FAC_SN,"\x00\x00\x00",3)==0)
            {
                bufferstrx-=12;
                if (memcmp(bufferstrx,"\x00\x00\x00",3)!=0)
                {
                    FAC_SN[0]=*bufferstrx;
                    FAC_SN[1]=*(bufferstrx+1);
                    FAC_SN[2]=*(bufferstrx+2);
                    Set_Fac_SN((char*)FAC_SN);
                    setID(send_package.ID);
                }
                bufferstrx+=12;
            }
						else
						{
							if (memcmp((bufferstrx-12),send_package.ID,sizeof(send_package.ID))!=0) break;
						}
            updateFileInfo.ver[0] = *bufferstrx;//0
            updateFileInfo.ver[1] = *(bufferstrx+1);//2
            updateFileInfo.ver[2] = *(bufferstrx+2);//4
						if (((uint32_t)updateFileInfo.ver[0]*256*256+(uint32_t)updateFileInfo.ver[1]*256+updateFileInfo.ver[2])>((uint32_t)SW_VER_1*256*256+(uint32_t)SW_VER_2*256+SW_VER_3)) daytime=Day_Times;
            bufferstrx+=3;
            tmpdat=*bufferstrx;
            tmpdat=(tmpdat<<8)+*(bufferstrx+1);
            tmpdat=(tmpdat<<8)+*(bufferstrx+2);
            tmpdat=(tmpdat<<8)+*(bufferstrx+3);
            bufferstrx+=4;
            if (tmpdat!=0) time.time_stamp=tmpdat;
            tmpdat=*bufferstrx;
            tmpdat=(tmpdat<<8)+*(bufferstrx+1);
            //tmpdat=(tmpdat<<8)+*(bufferstrx+2);
            //tmpdat=(tmpdat<<8)+*(bufferstrx+3);
            bufferstrx+=2;//bufferstrx+=4;
            if ((tmpdat!=0)&&(Dfrequency!=tmpdat)) 
						{
								Dfrequency=tmpdat;
								Set_Dfrequency(Dfrequency);
								SetDfrequency(Dfrequency);
						}

            if ((*((uint32_t *)(bufferstrx))!=0)&&(*((uint16_t *)(bufferstrx+4))!=0))
            {
								memcpy(tmpip,bufferstrx,4);
								memcpy(tmpport,bufferstrx+4,2);
							  if (memcmp(tmpip,ServerIP1,4)!=0)
								{
									memcpy(ServerIP1,bufferstrx,4);
									memcpy(ServerPort1,bufferstrx+4,2);
									Set_IPPort1((char *)ServerIP1,(char *)ServerPort1);
								}
            }
            bufferstrx+=6;

            if ((*((uint32_t *)(bufferstrx))!=0)&&(*((uint16_t *)(bufferstrx+4))!=0))
            {
								memcpy(tmpip,bufferstrx,4);
								memcpy(tmpport,bufferstrx+4,2);
							  if (memcmp(tmpip,ServerIP1,4)!=0)
								{
									memcpy(ServerIP2,bufferstrx,4);
									memcpy(ServerPort2,bufferstrx+4,2);
									Set_IPPort2((char *)ServerIP2,(char *)ServerPort2);
								}
            }
            bufferstrx+=6;
            /*
            设置的频率4
            生产日期3
            */
            if ((*((uint32_t *)(bufferstrx))!=0)&&(*((uint16_t *)(bufferstrx+4))!=0))
            {
                //设置频率
            }
            bufferstrx+=4;
            /*
            if (memcmp(bufferstrx,"\x00\x00\x00",3)!=0)
            {
            	FAC_SN[0]=*bufferstrx;
            	FAC_SN[1]=*(bufferstrx+1);
            	FAC_SN[2]=*(bufferstrx+2);
            	Set_Fac_SN((char*)FAC_SN);
            	setID(send_package.ID);
            }
            */
            bufferstrx+=3;
        }
        sensorDataSendState = true;
        GetRequestUpdateOK=1;
    }
    break;
    case MSG_TYPE_UPDATE_DATA://升级响应
        /*
        		设备号10 帧序号1字节 消息类型1
        		服务器端软件版本号（3字节）
        		本次数据的偏移地址4字节
        		本次数据的长度4字节
        		服务器端软件固件全体校验CRC32（4字节）
        		当前数据体CRC32校验（4字节）
        		数据内容（16进制，数据长度符合前面的本次长度）
        		*/
		
        //if (usart3RxCnt<(Recv_UPDATE_DATA_len+UPDATE_DOWN_REQ_LEN)) break;
        if (usart3RxCnt<Recv_UPDATE_DATA_len) break;
				if (memcmp(bufferstrx,send_package.ID,sizeof(send_package.ID))!=0) break;
        bufferstrx+=12;
        //if (IPType==IsFactoryIP)			//liyu test
        {
            ver[0] = *bufferstrx;//0
            ver[1] = *(bufferstrx+1);//2
            ver[2] = *(bufferstrx+2);//4
            bufferstrx+=3;
            if( !((ver[0]==updateFileInfo.ver[0]) && (ver[1]==updateFileInfo.ver[1]) && (ver[2]==updateFileInfo.ver[2])) )
            {
                Uart1_SendStr("Verison err！\r\n");
								updateFileInfo.reqOffset=0;
                break;
            }
            tmpdat=*bufferstrx;
            tmpdat=(tmpdat<<8)+*(bufferstrx+1);
            tmpdat=(tmpdat<<8)+*(bufferstrx+2);
            tmpdat=(tmpdat<<8)+*(bufferstrx+3);
            bufferstrx+=4;
            if (tmpdat!=0) offset=tmpdat;
            tmpdat=*bufferstrx;
            tmpdat=(tmpdat<<8)+*(bufferstrx+1);
            tmpdat=(tmpdat<<8)+*(bufferstrx+2);
            tmpdat=(tmpdat<<8)+*(bufferstrx+3);
            bufferstrx+=4;
            if (tmpdat!=0) dataLen=tmpdat;
            tmpdat=*bufferstrx;
            tmpdat=(tmpdat<<8)+*(bufferstrx+1);
            tmpdat=(tmpdat<<8)+*(bufferstrx+2);
            tmpdat=(tmpdat<<8)+*(bufferstrx+3);
            bufferstrx+=4;
            tmpcrc=tmpdat;
						if (updateFileInfo.crc!=tmpcrc)
						{
                Uart1_SendStr("总包校验和错！\r\n");
								updateFileInfo.reqOffset=0;
								return;
						}
						tmpdat=*bufferstrx;
            tmpdat=(tmpdat<<8)+*(bufferstrx+1);
            tmpdat=(tmpdat<<8)+*(bufferstrx+2);
            tmpdat=(tmpdat<<8)+*(bufferstrx+3);
            bufferstrx+=4;
            tmpcrc=tmpdat;
            if (dataLen!=0)	
						{
							init_crc_table(crc_table);		
							tmpcrc1 = 0xffffffff;
							tmpcrc1 = Crc32(crc_table, tmpcrc1,(uint8_t *)bufferstrx, dataLen);
							tmpcrc1 = tmpcrc1^0xffffffffL;
							if (tmpcrc1!=tmpcrc)
							{
									Uart1_SendStr("包校验和错！\r\n");
									return;
							}
							memcpy(TmpUpdateData,bufferstrx,dataLen);
							Mem_Write_Ex(APP_FILE_ADDR+offset, (uint8_t *)bufferstrx, dataLen);
							Mem_Read(APP_FILE_ADDR+offset,MemUpdateData, dataLen);
							if (memcmp(MemUpdateData,TmpUpdateData,dataLen)!=0)
							{
									Uart1_SendStr("写入失败！\r\n");
									return;
							}
							updateFileInfo.ackOffset = offset;
							GetRequestUpdateOK=1;
						}
            else
            {
								Uart1_SendStr("数据长度错！\r\n");
								return;
            }
        }
        break;

    default:
        break;
    }
}

/*******************************************************************************
* @Function Name	:静态变量setter, getter
* @Description	:  .
* @Input		: None
* @Output	: None
* @Return	: None
*******************************************************************************/
enum sys_state getState(void)
{
    return new_state;
}

void setState(enum sys_state state)
{
    new_state=state;
}

bool IsSensorDataSendSuccess(void)
{
    return sensorDataSendState;
}
void ResetSensorDataSendState(void)
{
    sensorDataSendState = false;
}
void SetSensorDataSendState(void)
{
    sensorDataSendState = true;
}

sys_time getSystime(void) {
    return time;
}

void ReSetSystime(void)
{
    time.time_stamp=0;
}

void SetSystime(uint32_t timestamp)
{
    time.time_stamp=timestamp;
}

void SystimeAdd20(void)
{
    time.time_stamp+=20;
}


/*******************************************************************************
* @Function Name	:接收数据函数
* @Description	:接收到数据则解析，并将字符按照ANSI转码.
* @Input		: None
* @Output	: None
* @Return	: None
*******************************************************************************/
void LoraRcvHandler(uint8_t IPType)
{
    Uart1_SendStr("RecvLora：");
    Uart1_SendHEXData(LoraRxBuffer,LoraRxCnt);
    Uart1_SendStr("\r\n");
    RecvServerPackage(LoraRxCnt,(uint8_t *)LoraRxBuffer,IPType);
    ClrLoraBuffer();
}



