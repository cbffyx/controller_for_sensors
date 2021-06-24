#include "ProUart1.h"

extern char usart1RxBuffer[usart1RxBufferSize];
extern uint16_t usart1RxCnt;
extern SensorSInfo sensorSInfo;
extern SensorSInfo sensorReal;
extern uint8_t FAC_SN[3];
extern void Set_Fac_SN(char* SN);
extern package send_package;

uint8_t ReadBiaoDingData(uint32_t Addr,BD *Data)
{
    uint8_t buf[MaxBDCount*8+2];
    uint8_t i;

    Mem_Read(Addr, buf, MaxBDCount*8+2);
    memcpy(Data,buf,MaxBDCount*8+2);
    if (Data->BdFlag!=0xAA)
    {
        Data->BdFlag=0xAA;
        Data->BdCount=0;
        for (i=0; i<MaxBDCount; i++)
        {
            Data->BdData[i].Bdata=0;
            Data->BdData[i].Rdata=0;
        }
        memcpy(buf,Data,MaxBDCount*8+2);
        Mem_Erase( Addr, W25Q128FV_SUBSECTOR_SIZE);
        Mem_Write_Ex( Addr, buf, MaxBDCount*8+2);
    }
    return Data->BdCount;
}

uint8_t AddBD(uint32_t Addr,float RealData,float NowData)//加温度标定点
{
    uint8_t i,j;
    BD BiaoDingData;
    uint8_t buf[MaxBDCount*8+2];

    ReadBiaoDingData(Addr,&BiaoDingData);
    //获取当前标定点数
    if (BiaoDingData.BdCount>=MaxBDCount) return 1;//超出标定点数
    if (BiaoDingData.BdCount==0) //当前未标定
    {
        BiaoDingData.BdData[0].Rdata=RealData;
        BiaoDingData.BdData[0].Bdata=NowData;
        BiaoDingData.BdCount=1;
        memcpy(buf,&BiaoDingData,MaxBDCount*8+2);
        Mem_Erase( Addr, W25Q128FV_SUBSECTOR_SIZE);
        Mem_Write_Ex( Addr, buf, MaxBDCount*8+2);
    }
    else
    {
        for (i=0; i<BiaoDingData.BdCount; i++)
        {
            //if (BiaoDingData.BdData[i].Rdata<RealData) continue;//20200107标定修改原来只能先标定大数，后标定小数；变更为随意标定。
            if (BiaoDingData.BdData[i].Rdata<RealData)
            {
                if ((i+1)!=BiaoDingData.BdCount)//不是最后一个
                {
                    continue;
                }
                else
                {   //是最后一个
                    BiaoDingData.BdData[BiaoDingData.BdCount].Rdata=RealData;
                    BiaoDingData.BdData[BiaoDingData.BdCount].Bdata=NowData;
                    BiaoDingData.BdCount++;
                    memcpy(buf,&BiaoDingData,MaxBDCount*8+2);
                    Mem_Erase( Addr, W25Q128FV_SUBSECTOR_SIZE);
                    Mem_Write_Ex( Addr, buf, MaxBDCount*8+2);
                    return 0;
                }
            }
            else if (BiaoDingData.BdData[i].Rdata==RealData) //重复标定
            {
                BiaoDingData.BdData[i].Rdata=RealData;//覆盖原来的标定
                BiaoDingData.BdData[i].Bdata=NowData;
                memcpy(buf,&BiaoDingData,MaxBDCount*8+2);
                Mem_Erase( Addr, W25Q128FV_SUBSECTOR_SIZE);
                Mem_Write_Ex( Addr, buf, MaxBDCount*8+2);
                return 0;
            }
            else
            {
                for (j=BiaoDingData.BdCount; j>i; j--)
                {
                    BiaoDingData.BdData[j].Rdata=BiaoDingData.BdData[j-1].Rdata;
                    BiaoDingData.BdData[j].Bdata=BiaoDingData.BdData[j-1].Bdata;
                }
                BiaoDingData.BdData[i].Rdata=RealData;
                BiaoDingData.BdData[i].Bdata=NowData;
                BiaoDingData.BdCount++;
                memcpy(buf,&BiaoDingData,MaxBDCount*8+2);
                Mem_Erase( Addr, W25Q128FV_SUBSECTOR_SIZE);
                Mem_Write_Ex( Addr, buf, MaxBDCount*8+2);
                return 0;
            }
        }
        BiaoDingData.BdData[BiaoDingData.BdCount].Rdata=RealData;
        BiaoDingData.BdData[BiaoDingData.BdCount].Bdata=NowData;
        BiaoDingData.BdCount++;
        memcpy(buf,&BiaoDingData,MaxBDCount*8+2);
        Mem_Erase( Addr, W25Q128FV_SUBSECTOR_SIZE);
        Mem_Write_Ex( Addr, buf, MaxBDCount*8+2);
    }
    return 0;
}

float JS(uint32_t Addr,float data)
{
    float result;
    BD BiaoDingData;
    uint8_t i,index;

    ReadBiaoDingData(Addr,&BiaoDingData);
    index=BiaoDingData.BdCount;
    if (BiaoDingData.BdCount==0)
    {
        result=data;
    }
    else if (BiaoDingData.BdCount==1)
    {
        {   //平移
            result=(BiaoDingData.BdData[0].Bdata-BiaoDingData.BdData[0].Rdata+data);
        }
    }
    else
    {
        for (i=0; i<BiaoDingData.BdCount; i++)
        {
            if (BiaoDingData.BdData[i].Rdata>data)
            {
                index=i;
                break;
            }
        }
        if (index==0) //index  index+1  y=(x-x1)*(y2-y1)/(x2-x1)+y1
        {
            result=(data-BiaoDingData.BdData[index].Rdata)*(BiaoDingData.BdData[index+1].Bdata-BiaoDingData.BdData[index].Bdata)/(BiaoDingData.BdData[index+1].Rdata-BiaoDingData.BdData[index].Rdata)+BiaoDingData.BdData[index].Bdata;
        }
        else if (index==BiaoDingData.BdCount)//index-2  index-1
        {
            //(x-x1)/(x2-x1)=(y-y1)/(y2-y1) y-y1=(x-x1)*(y2-y1)/(x2-x1) y=(x-x1)*(y2-y1)/(x2-x1)+y1
            result=(data-BiaoDingData.BdData[index-2].Rdata)*(BiaoDingData.BdData[index-1].Bdata-BiaoDingData.BdData[index-2].Bdata)/(BiaoDingData.BdData[index-1].Rdata-BiaoDingData.BdData[index-2].Rdata)+BiaoDingData.BdData[index-2].Bdata;
        }
        else//index-1 index
        {
            result=(data-BiaoDingData.BdData[index-1].Rdata)*(BiaoDingData.BdData[index].Bdata-BiaoDingData.BdData[index-1].Bdata)/(BiaoDingData.BdData[index].Rdata-BiaoDingData.BdData[index-1].Rdata)+BiaoDingData.BdData[index-1].Bdata;
        }
    }

    return result;
}

void ClearBD(uint32_t Addr)
{
    BD BiaoDingData;
    uint8_t buf[MaxBDCount*8+2];
    uint8_t i;

    BiaoDingData.BdFlag=0xAA;
    BiaoDingData.BdCount=0;
    for (i=0; i<MaxBDCount; i++)
    {
        BiaoDingData.BdData[i].Bdata=0;
        BiaoDingData.BdData[i].Rdata=0;
    }
    memcpy(buf,&BiaoDingData,MaxBDCount*8+2);
    Mem_Erase( Addr, W25Q128FV_SUBSECTOR_SIZE);
    Mem_Write_Ex( Addr, buf, MaxBDCount*8+2);
}

void Read_APPEUI_APPKEY(char * APPEUI,char * APPKEY)
{
    Mem_Read(Loar_Info_Addr, (uint8_t *)APPEUI, APPEUI_Len);
    Mem_Read(Loar_Info_Addr+APPEUI_Len, (uint8_t *)APPKEY, APPKEY_Len);
    *(APPEUI+APPEUI_Len)=0;
    *(APPKEY+APPKEY_Len)=0;
}

void Write_APPEUI_APPKEY(char * APPEUI,char * APPKEY)
{
    uint8_t buf[APPEUI_Len+APPKEY_Len];
    memcpy(buf,APPEUI,APPEUI_Len);
    memcpy(&buf[APPEUI_Len],APPKEY,APPKEY_Len);
    Mem_Erase( Loar_Info_Addr, W25Q128FV_SUBSECTOR_SIZE);
    Mem_Write_Ex( Loar_Info_Addr, buf, APPEUI_Len+APPKEY_Len);
}

bool DealUart(uint8_t *RecFuf,uint16_t RcvLen)
{
    uint16_t len,len1;
    char *p=NULL;
    char *p1=NULL;
    char *p2=NULL;
    float RealData;
    bool result;
    uint32_t BDtype;

    result=false;
    p=(char *)GetStringMiddle((uint8_t *)usart1RxBuffer, usart1RxCnt, '{', 1, '}', 1, &len);
    if (p==NULL)
    {
        if (usart1RxCnt>=usart1RxBufferSize-2) usart1RxCnt=0;
        return result;
    }

    p1=strstr((const char*)p, (const char*)"\"ct1\"");//查找温度标识
    p2=(char *)GetStringMiddle((uint8_t *)p1, len, ':', 1, ',', 1, &len1);
    if (p2==NULL) p2=(char *)GetStringMiddle((uint8_t *)p1, len+2, ':', 1, '}', 1, &len1);
    if (p2!=NULL)
    {
        sscanf(p2,"%d",&BDtype);//得到当前温度控制类型
        switch(BDtype)
        {
        case 0://标定
            p1=strstr((const char*)p, (const char*)"\"t1\"");//查找温度值标识
            p2=(char *)GetStringMiddle((uint8_t *)p1, len, ':', 1, ',', 1, &len1);
            if (p2==NULL) p2=(char *)GetStringMiddle((uint8_t *)p1, len+2, ':', 1, '}', 1, &len1);
            if (p2!=NULL)
            {
                sscanf(p2,"%f",&RealData);//得到当前温度值
                if (AddBD(WD_BD_Addr1,sensorReal.temp1,RealData)==0)
                {
                    sensorSInfo.temp1=JS(WD_BD_Addr1,sensorReal.temp1);//重新计算温度
                    result=true;
                }
            }
            break;
        case 1://不标定
            break;
        case 2://清除
            //清除温度标定
            ClearBD(WD_BD_Addr1);
            sensorSInfo.temp1=JS(WD_BD_Addr1,sensorReal.temp1);//重新计算温度
            result=true;
            break;
        default:
            break;
        }

    }

    p1=strstr((const char*)p, (const char*)"\"ch1\"");//查找湿度标识
    p2=(char *)GetStringMiddle((uint8_t *)p1, len, ':', 1, ',', 1, &len1);
    if (p2==NULL) p2=(char *)GetStringMiddle((uint8_t *)p1, len+2, ':', 1, '}', 1, &len1);
    if (p2!=NULL)
    {
        sscanf(p2,"%d",&BDtype);//得到当前湿度控制类型
        switch(BDtype)
        {
        case 0://标定
            p1=strstr((const char*)p, (const char*)"\"h1\"");//查找湿度值标识
            p2=(char *)GetStringMiddle((uint8_t *)p1, len, ':', 1, ',', 1, &len1);
            if (p2==NULL) p2=(char *)GetStringMiddle((uint8_t *)p1, len+2, ':', 1, '}', 1, &len1);
            if (p2!=NULL)
            {
                sscanf(p2,"%f",&RealData);//得到当前湿度值
                if (AddBD(SD_BD_Addr1,sensorReal.humi1,RealData)==0)
                {
                    sensorSInfo.humi1=JS(SD_BD_Addr1,sensorReal.humi1);
                    result=true;
                }
            }
            break;
        case 1://不标定
            break;
        case 2://清除
            //清除湿度标定
            ClearBD(SD_BD_Addr1);
            sensorSInfo.humi1=JS(SD_BD_Addr1,sensorReal.humi1);
            result=true;
            break;
        default:
            break;
        }

    }

    p1=strstr((const char*)p, (const char*)"\"ct2\"");//查找ct2标识
    p2=(char *)GetStringMiddle((uint8_t *)p1, len, ':', 1, ',', 1, &len1);
    if (p2==NULL) p2=(char *)GetStringMiddle((uint8_t *)p1, len+2, ':', 1, '}', 1, &len1);
    if (p2!=NULL)
    {
        sscanf(p2,"%d",&BDtype);//得到当前ct2控制类型
        switch(BDtype)
        {
        case 0://标定
            p1=strstr((const char*)p, (const char*)"\"t2\"");//查找ct2值标识
            p2=(char *)GetStringMiddle((uint8_t *)p1, len, ':', 1, ',', 1, &len1);
            if (p2==NULL) p2=(char *)GetStringMiddle((uint8_t *)p1, len+2, ':', 1, '}', 1, &len1);
            if (p2!=NULL)
            {
                sscanf(p2,"%f",&RealData);//得到当前ct2值
                if (AddBD(WD_BD_Addr2,sensorReal.temp2,RealData)==0)
                {
                    sensorSInfo.temp2=JS(WD_BD_Addr2,sensorReal.temp2);
                    result=true;
                }
            }
            break;
        case 1://不标定
            break;
        case 2://清除
            //清除PH32K标定
            ClearBD(WD_BD_Addr2);
            sensorSInfo.temp2=JS(WD_BD_Addr2,sensorReal.temp2);
            result=true;
            break;
        default:
            break;
        }

    }

    p1=strstr((const char*)p, (const char*)"\"ch2\"");//查找ch2标识
    p2=(char *)GetStringMiddle((uint8_t *)p1, len, ':', 1, ',', 1, &len1);
    if (p2==NULL) p2=(char *)GetStringMiddle((uint8_t *)p1, len+2, ':', 1, '}', 1, &len1);
    if (p2!=NULL)
    {
        sscanf(p2,"%d",&BDtype);//得到当前PH35控制类型
        switch(BDtype)
        {
        case 0://标定
            p1=strstr((const char*)p, (const char*)"\"h2\"");//查找PH35值标识
            p2=(char *)GetStringMiddle((uint8_t *)p1, len, ':', 1, ',', 1, &len1);
            if (p2==NULL) p2=(char *)GetStringMiddle((uint8_t *)p1, len+2, ':', 1, '}', 1, &len1);
            if (p2!=NULL)
            {
                sscanf(p2,"%f",&RealData);//得到当前PH35值
                if (AddBD(SD_BD_Addr2,sensorReal.humi2,RealData)==0)
                {
                    sensorSInfo.humi2=JS(SD_BD_Addr2,sensorReal.humi2);
                    result=true;
                }
            }
            break;
        case 1://不标定
            break;
        case 2://清除
            //清除PH35标定
            ClearBD(SD_BD_Addr2);
            sensorSInfo.humi2=JS(SD_BD_Addr2,sensorReal.humi2);
            result=true;
            break;
        default:
            break;
        }

    }

    p1=strstr((const char*)p, (const char*)"\"ct3\"");//查找ct3标识
    p2=(char *)GetStringMiddle((uint8_t *)p1, len, ':', 1, ',', 1, &len1);
    if (p2==NULL) p2=(char *)GetStringMiddle((uint8_t *)p1, len+2, ':', 1, '}', 1, &len1);
    if (p2!=NULL)
    {
        sscanf(p2,"%d",&BDtype);//得到当前ct2控制类型
        switch(BDtype)
        {
        case 0://标定
            p1=strstr((const char*)p, (const char*)"\"t3\"");//查找ct2值标识
            p2=(char *)GetStringMiddle((uint8_t *)p1, len, ':', 1, ',', 1, &len1);
            if (p2==NULL) p2=(char *)GetStringMiddle((uint8_t *)p1, len+2, ':', 1, '}', 1, &len1);
            if (p2!=NULL)
            {
                sscanf(p2,"%f",&RealData);//得到当前ct2值
                if (AddBD(WD_BD_Addr3,sensorReal.temp3,RealData)==0)
                {
                    sensorSInfo.temp3=JS(WD_BD_Addr3,sensorReal.temp3);
                    result=true;
                }
            }
            break;
        case 1://不标定
            break;
        case 2://清除
            //清除PH32K标定
            ClearBD(WD_BD_Addr3);
            sensorSInfo.temp3=JS(WD_BD_Addr3,sensorReal.temp3);
            result=true;
            break;
        default:
            break;
        }

    }

    p1=strstr((const char*)p, (const char*)"\"ch3\"");//查找ch3标识
    p2=(char *)GetStringMiddle((uint8_t *)p1, len, ':', 1, ',', 1, &len1);
    if (p2==NULL) p2=(char *)GetStringMiddle((uint8_t *)p1, len+2, ':', 1, '}', 1, &len1);
    if (p2!=NULL)
    {
        sscanf(p2,"%d",&BDtype);//得到当前PH35控制类型
        switch(BDtype)
        {
        case 0://标定
            p1=strstr((const char*)p, (const char*)"\"h3\"");//查找PH35值标识
            p2=(char *)GetStringMiddle((uint8_t *)p1, len, ':', 1, ',', 1, &len1);
            if (p2==NULL) p2=(char *)GetStringMiddle((uint8_t *)p1, len+2, ':', 1, '}', 1, &len1);
            if (p2!=NULL)
            {
                sscanf(p2,"%f",&RealData);//得到当前PH35值
                if (AddBD(SD_BD_Addr3,sensorReal.humi3,RealData)==0)
                {
                    sensorSInfo.humi3=JS(SD_BD_Addr2,sensorReal.humi3);
                    result=true;
                }
            }
            break;
        case 1://不标定
            break;
        case 2://清除
            //清除PH35标定
            ClearBD(SD_BD_Addr3);
            sensorSInfo.humi3=JS(SD_BD_Addr3,sensorReal.humi3);
            result=true;
            break;
        default:
            break;
        }

    }

    p1=strstr((const char*)p, (const char*)"\"cp\"");//查找气压标识
    p2=(char *)GetStringMiddle((uint8_t *)p1, len, ':', 1, ',', 1, &len1);
    if (p2==NULL) p2=(char *)GetStringMiddle((uint8_t *)p1, len+2, ':', 1, '}', 1, &len1);
    if (p2!=NULL)
    {
        sscanf(p2,"%d",&BDtype);//得到当前气压控制类型
        switch(BDtype)
        {
        case 0://标定
            p1=strstr((const char*)p, (const char*)"\"p\"");//查找气压值标识
            p2=(char *)GetStringMiddle((uint8_t *)p1, len, ':', 1, ',', 1, &len1);
            if (p2==NULL) p2=(char *)GetStringMiddle((uint8_t *)p1, len+2, ':', 1, '}', 1, &len1);
            if (p2!=NULL)
            {
                sscanf(p2,"%f",&RealData);//得到当前气压值
                if (AddBD(QY_BD_Addr,sensorReal.press,RealData)==0)
                {
                    sensorSInfo.press=JS(QY_BD_Addr,sensorReal.press);
                    result=true;
                }
            }
            break;
        case 1://不标定
            break;
        case 2://清除
            //清除气压标定
            ClearBD(QY_BD_Addr);
            sensorSInfo.press=JS(QY_BD_Addr,sensorReal.press);
            result=true;
            break;
        default:
            break;
        }

    }

    p1=strstr((const char*)p, (const char*)"\"cm\"");//查找水份标识
    p2=(char *)GetStringMiddle((uint8_t *)p1, len, ':', 1, ',', 1, &len1);
    if (p2==NULL) p2=(char *)GetStringMiddle((uint8_t *)p1, len+2, ':', 1, '}', 1, &len1);
    if (p2!=NULL)
    {
        sscanf(p2,"%d",&BDtype);//得到当前水份控制类型
        switch(BDtype)
        {
        case 0://标定
            p1=strstr((const char*)p, (const char*)"\"m\"");//查找水份值标识
            p2=(char *)GetStringMiddle((uint8_t *)p1, len, ':', 1, ',', 1, &len1);
            if (p2==NULL) p2=(char *)GetStringMiddle((uint8_t *)p1, len+2, ':', 1, '}', 1, &len1);
            if (p2!=NULL)
            {
                sscanf(p2,"%f",&RealData);//得到当前水份值
                if (AddBD(SF_BD_Addr,sensorReal.moisture,RealData)==0)
                {
                    sensorSInfo.moisture=JS(SF_BD_Addr,sensorReal.moisture);
                    result=true;
                }
            }
            break;
        case 1://不标定
            break;
        case 2://清除
            //清除水份标定
            ClearBD(SF_BD_Addr);
            sensorSInfo.moisture=JS(SF_BD_Addr,sensorReal.moisture);
            result=true;
            break;
        default:
            break;
        }

    }

    if (memcmp(FAC_SN,"\x00\x00\x00",3)==0)
    {
        p1=strstr((const char*)p, (const char*)"\"SN\"");//查找气压标识
        p2=(char *)GetStringMiddle((uint8_t *)p1, len, ':', 1, ',', 1, &len1);
        if (p2==NULL) p2=(char *)GetStringMiddle((uint8_t *)p1, len+2, ':', 1, '}', 1, &len1);
        if (p2!=NULL)
        {
						p2++;
						FAC_SN[0]=StrToHex(*p2);p2++;
						FAC_SN[0]=(FAC_SN[0]<<4)+StrToHex(*p2);p2++;
						FAC_SN[1]=StrToHex(*p2);p2++;
						FAC_SN[1]=(FAC_SN[1]<<4)+StrToHex(*p2);p2++;
						FAC_SN[2]=StrToHex(*p2);p2++;
						FAC_SN[2]=(FAC_SN[2]<<4)+StrToHex(*p2);p2++;
            Set_Fac_SN((char*)FAC_SN);
            setID(send_package.ID);
        }
    }

    p1=strstr((const char*)p, (const char*)"\"AE\"");//查找APPEUI标识
    p2=(char *)GetStringMiddle((uint8_t *)p1, len, ':', 1, ',', 1, &len1);
    if (p2==NULL) p2=(char *)GetStringMiddle((uint8_t *)p1, len+2, ':', 1, '}', 1, &len1);
    if (p2!=NULL)
    {
        memcpy(LoRaNode.AppEUI,p2+1,APPEUI_Len);
        LoRaNode.AppEUI[APPEUI_Len]=0;
        Write_APPEUI_APPKEY((char *)LoRaNode.AppEUI,(char *)LoRaNode.AppKEY);
    }
    p1=strstr((const char*)p, (const char*)"\"AK\"");//查找APPKEY标识
    p2=(char *)GetStringMiddle((uint8_t *)p1, len, ':', 1, ',', 1, &len1);
    if (p2==NULL) p2=(char *)GetStringMiddle((uint8_t *)p1, len+2, ':', 1, '}', 1, &len1);
    if (p2!=NULL)
    {
        memcpy(LoRaNode.AppKEY,p2+1,APPKEY_Len);
        LoRaNode.AppKEY[APPKEY_Len]=0;
        Write_APPEUI_APPKEY((char *)LoRaNode.AppEUI,(char *)LoRaNode.AppKEY);
    }

    p1=strstr((const char*)p, (const char*)"\"QZSN\"");//
		p2=(char *)GetStringMiddle((uint8_t *)p1, len, ':', 1, ',', 1, &len1);
		if (p2==NULL) p2=(char *)GetStringMiddle((uint8_t *)p1, len+2, ':', 1, '}', 1, &len1);
		if (p2!=NULL)
		{
				p2++;
				FAC_SN[0]=StrToHex(*p2);p2++;
				FAC_SN[0]=(FAC_SN[0]<<4)+StrToHex(*p2);p2++;
				FAC_SN[1]=StrToHex(*p2);p2++;
				FAC_SN[1]=(FAC_SN[1]<<4)+StrToHex(*p2);p2++;
				FAC_SN[2]=StrToHex(*p2);p2++;
				FAC_SN[2]=(FAC_SN[2]<<4)+StrToHex(*p2);p2++;
				Set_Fac_SN((char*)FAC_SN);
				setID(send_package.ID);
		}

    p1=strstr((const char*)p, (const char*)"\"QZClearRecord\"");//
		p2=(char *)GetStringMiddle((uint8_t *)p1, len, ':', 1, ',', 1, &len1);
		if (p2==NULL) p2=(char *)GetStringMiddle((uint8_t *)p1, len+2, ':', 1, '}', 1, &len1);
		if (p2!=NULL)
		{
			ClearSensorMemSafe();
		}

    p1=strstr((const char*)p, (const char*)"\"QZClearMemAll\"");//
		p2=(char *)GetStringMiddle((uint8_t *)p1, len, ':', 1, ',', 1, &len1);
		if (p2==NULL) p2=(char *)GetStringMiddle((uint8_t *)p1, len+2, ':', 1, '}', 1, &len1);
		if (p2!=NULL)
		{
			BSP_W25Qx_Erase_Chip();
		}

    usart1RxCnt=0;
    return result;
}

/*
    float temp;//4
    float humi;//4
    float ph3_2000;//4
    float ph3_5;//4
    float press;//4
		float moisture;//4
    uint8_t batPercent;//1   25
    uint32_t ts;// 29
    uint8_t dig;//30
    uint8_t devicestatus;//31
		uint8_t runstatus;//32
		uint8_t protect;//33
*/
void BDConvert(void)
{
    sensorSInfo.batPercent=sensorReal.batPercent;

    sensorSInfo.temp1=JS(WD_BD_Addr1,sensorReal.temp1);//重新计算温度
    sensorSInfo.humi1=JS(SD_BD_Addr1,sensorReal.humi1);
    sensorSInfo.temp2=JS(WD_BD_Addr2,sensorReal.temp2);
    sensorSInfo.humi2=JS(SD_BD_Addr2,sensorReal.humi2);
    sensorSInfo.temp3=JS(WD_BD_Addr3,sensorReal.temp3);
    sensorSInfo.humi3=JS(SD_BD_Addr3,sensorReal.humi3);
    sensorSInfo.press=JS(QY_BD_Addr,sensorReal.press);
    sensorSInfo.moisture=JS(SF_BD_Addr,sensorReal.moisture);

 if ((sensorSInfo.temp1>9.5) &&(sensorSInfo.temp1<11)) sensorSInfo.temp1-=0.32;
 if ((sensorSInfo.temp2>9.5) &&(sensorSInfo.temp2<11)) sensorSInfo.temp2-=0.2;
 if ((sensorSInfo.temp3>9.5) &&(sensorSInfo.temp3<11)) sensorSInfo.temp3-=0.4;
 if ((sensorSInfo.temp1>19.5) &&(sensorSInfo.temp1<21)) sensorSInfo.temp1-=0.3;
 if ((sensorSInfo.temp2>19.5) &&(sensorSInfo.temp2<21)) sensorSInfo.temp2-=0.28;
 if ((sensorSInfo.temp3>19.5) &&(sensorSInfo.temp3<21)) sensorSInfo.temp3-=0.38;
 if ((sensorSInfo.temp1>29.5) &&(sensorSInfo.temp1<31)) sensorSInfo.temp1-=0.48;
 if ((sensorSInfo.temp2>29.5) &&(sensorSInfo.temp2<31)) sensorSInfo.temp2-=0.46;
 if ((sensorSInfo.temp3>29.5) &&(sensorSInfo.temp3<31)) sensorSInfo.temp3-=0.65;
 
 if ((sensorSInfo.humi1>43) &&(sensorSInfo.humi1<51)) sensorSInfo.humi1-=6.5;
 if ((sensorSInfo.humi2>43) &&(sensorSInfo.humi2<51)) sensorSInfo.humi2-=6.4;
 if ((sensorSInfo.humi3>43) &&(sensorSInfo.humi3<51)) sensorSInfo.humi3-=6.8;
 if ((sensorSInfo.humi1>60) &&(sensorSInfo.humi1<71)) sensorSInfo.humi1-=4.18;
 if ((sensorSInfo.humi2>60) &&(sensorSInfo.humi2<71)) sensorSInfo.humi2-=4.07;
 if ((sensorSInfo.humi3>60) &&(sensorSInfo.humi3<71)) sensorSInfo.humi3-=4.48;
 if ((sensorSInfo.humi1>78) &&(sensorSInfo.humi1<88)) sensorSInfo.humi1-=1.4;
 if ((sensorSInfo.humi2>78) &&(sensorSInfo.humi2<88)) sensorSInfo.humi2-=1.29;
 if ((sensorSInfo.humi3>78) &&(sensorSInfo.humi3<88)) sensorSInfo.humi3-=1.7;
}

