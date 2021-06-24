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

uint8_t AddBD(uint32_t Addr,float RealData,float NowData)//���¶ȱ궨��
{
    uint8_t i,j;
    BD BiaoDingData;
    uint8_t buf[MaxBDCount*8+2];

    ReadBiaoDingData(Addr,&BiaoDingData);
    //��ȡ��ǰ�궨����
    if (BiaoDingData.BdCount>=MaxBDCount) return 1;//�����궨����
    if (BiaoDingData.BdCount==0) //��ǰδ�궨
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
            //if (BiaoDingData.BdData[i].Rdata<RealData) continue;//20200107�궨�޸�ԭ��ֻ���ȱ궨��������궨С�������Ϊ����궨��
            if (BiaoDingData.BdData[i].Rdata<RealData)
            {
                if ((i+1)!=BiaoDingData.BdCount)//�������һ��
                {
                    continue;
                }
                else
                {   //�����һ��
                    BiaoDingData.BdData[BiaoDingData.BdCount].Rdata=RealData;
                    BiaoDingData.BdData[BiaoDingData.BdCount].Bdata=NowData;
                    BiaoDingData.BdCount++;
                    memcpy(buf,&BiaoDingData,MaxBDCount*8+2);
                    Mem_Erase( Addr, W25Q128FV_SUBSECTOR_SIZE);
                    Mem_Write_Ex( Addr, buf, MaxBDCount*8+2);
                    return 0;
                }
            }
            else if (BiaoDingData.BdData[i].Rdata==RealData) //�ظ��궨
            {
                BiaoDingData.BdData[i].Rdata=RealData;//����ԭ���ı궨
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
        {   //ƽ��
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

    p1=strstr((const char*)p, (const char*)"\"ct1\"");//�����¶ȱ�ʶ
    p2=(char *)GetStringMiddle((uint8_t *)p1, len, ':', 1, ',', 1, &len1);
    if (p2==NULL) p2=(char *)GetStringMiddle((uint8_t *)p1, len+2, ':', 1, '}', 1, &len1);
    if (p2!=NULL)
    {
        sscanf(p2,"%d",&BDtype);//�õ���ǰ�¶ȿ�������
        switch(BDtype)
        {
        case 0://�궨
            p1=strstr((const char*)p, (const char*)"\"t1\"");//�����¶�ֵ��ʶ
            p2=(char *)GetStringMiddle((uint8_t *)p1, len, ':', 1, ',', 1, &len1);
            if (p2==NULL) p2=(char *)GetStringMiddle((uint8_t *)p1, len+2, ':', 1, '}', 1, &len1);
            if (p2!=NULL)
            {
                sscanf(p2,"%f",&RealData);//�õ���ǰ�¶�ֵ
                if (AddBD(WD_BD_Addr1,sensorReal.temp1,RealData)==0)
                {
                    sensorSInfo.temp1=JS(WD_BD_Addr1,sensorReal.temp1);//���¼����¶�
                    result=true;
                }
            }
            break;
        case 1://���궨
            break;
        case 2://���
            //����¶ȱ궨
            ClearBD(WD_BD_Addr1);
            sensorSInfo.temp1=JS(WD_BD_Addr1,sensorReal.temp1);//���¼����¶�
            result=true;
            break;
        default:
            break;
        }

    }

    p1=strstr((const char*)p, (const char*)"\"ch1\"");//����ʪ�ȱ�ʶ
    p2=(char *)GetStringMiddle((uint8_t *)p1, len, ':', 1, ',', 1, &len1);
    if (p2==NULL) p2=(char *)GetStringMiddle((uint8_t *)p1, len+2, ':', 1, '}', 1, &len1);
    if (p2!=NULL)
    {
        sscanf(p2,"%d",&BDtype);//�õ���ǰʪ�ȿ�������
        switch(BDtype)
        {
        case 0://�궨
            p1=strstr((const char*)p, (const char*)"\"h1\"");//����ʪ��ֵ��ʶ
            p2=(char *)GetStringMiddle((uint8_t *)p1, len, ':', 1, ',', 1, &len1);
            if (p2==NULL) p2=(char *)GetStringMiddle((uint8_t *)p1, len+2, ':', 1, '}', 1, &len1);
            if (p2!=NULL)
            {
                sscanf(p2,"%f",&RealData);//�õ���ǰʪ��ֵ
                if (AddBD(SD_BD_Addr1,sensorReal.humi1,RealData)==0)
                {
                    sensorSInfo.humi1=JS(SD_BD_Addr1,sensorReal.humi1);
                    result=true;
                }
            }
            break;
        case 1://���궨
            break;
        case 2://���
            //���ʪ�ȱ궨
            ClearBD(SD_BD_Addr1);
            sensorSInfo.humi1=JS(SD_BD_Addr1,sensorReal.humi1);
            result=true;
            break;
        default:
            break;
        }

    }

    p1=strstr((const char*)p, (const char*)"\"ct2\"");//����ct2��ʶ
    p2=(char *)GetStringMiddle((uint8_t *)p1, len, ':', 1, ',', 1, &len1);
    if (p2==NULL) p2=(char *)GetStringMiddle((uint8_t *)p1, len+2, ':', 1, '}', 1, &len1);
    if (p2!=NULL)
    {
        sscanf(p2,"%d",&BDtype);//�õ���ǰct2��������
        switch(BDtype)
        {
        case 0://�궨
            p1=strstr((const char*)p, (const char*)"\"t2\"");//����ct2ֵ��ʶ
            p2=(char *)GetStringMiddle((uint8_t *)p1, len, ':', 1, ',', 1, &len1);
            if (p2==NULL) p2=(char *)GetStringMiddle((uint8_t *)p1, len+2, ':', 1, '}', 1, &len1);
            if (p2!=NULL)
            {
                sscanf(p2,"%f",&RealData);//�õ���ǰct2ֵ
                if (AddBD(WD_BD_Addr2,sensorReal.temp2,RealData)==0)
                {
                    sensorSInfo.temp2=JS(WD_BD_Addr2,sensorReal.temp2);
                    result=true;
                }
            }
            break;
        case 1://���궨
            break;
        case 2://���
            //���PH32K�궨
            ClearBD(WD_BD_Addr2);
            sensorSInfo.temp2=JS(WD_BD_Addr2,sensorReal.temp2);
            result=true;
            break;
        default:
            break;
        }

    }

    p1=strstr((const char*)p, (const char*)"\"ch2\"");//����ch2��ʶ
    p2=(char *)GetStringMiddle((uint8_t *)p1, len, ':', 1, ',', 1, &len1);
    if (p2==NULL) p2=(char *)GetStringMiddle((uint8_t *)p1, len+2, ':', 1, '}', 1, &len1);
    if (p2!=NULL)
    {
        sscanf(p2,"%d",&BDtype);//�õ���ǰPH35��������
        switch(BDtype)
        {
        case 0://�궨
            p1=strstr((const char*)p, (const char*)"\"h2\"");//����PH35ֵ��ʶ
            p2=(char *)GetStringMiddle((uint8_t *)p1, len, ':', 1, ',', 1, &len1);
            if (p2==NULL) p2=(char *)GetStringMiddle((uint8_t *)p1, len+2, ':', 1, '}', 1, &len1);
            if (p2!=NULL)
            {
                sscanf(p2,"%f",&RealData);//�õ���ǰPH35ֵ
                if (AddBD(SD_BD_Addr2,sensorReal.humi2,RealData)==0)
                {
                    sensorSInfo.humi2=JS(SD_BD_Addr2,sensorReal.humi2);
                    result=true;
                }
            }
            break;
        case 1://���궨
            break;
        case 2://���
            //���PH35�궨
            ClearBD(SD_BD_Addr2);
            sensorSInfo.humi2=JS(SD_BD_Addr2,sensorReal.humi2);
            result=true;
            break;
        default:
            break;
        }

    }

    p1=strstr((const char*)p, (const char*)"\"ct3\"");//����ct3��ʶ
    p2=(char *)GetStringMiddle((uint8_t *)p1, len, ':', 1, ',', 1, &len1);
    if (p2==NULL) p2=(char *)GetStringMiddle((uint8_t *)p1, len+2, ':', 1, '}', 1, &len1);
    if (p2!=NULL)
    {
        sscanf(p2,"%d",&BDtype);//�õ���ǰct2��������
        switch(BDtype)
        {
        case 0://�궨
            p1=strstr((const char*)p, (const char*)"\"t3\"");//����ct2ֵ��ʶ
            p2=(char *)GetStringMiddle((uint8_t *)p1, len, ':', 1, ',', 1, &len1);
            if (p2==NULL) p2=(char *)GetStringMiddle((uint8_t *)p1, len+2, ':', 1, '}', 1, &len1);
            if (p2!=NULL)
            {
                sscanf(p2,"%f",&RealData);//�õ���ǰct2ֵ
                if (AddBD(WD_BD_Addr3,sensorReal.temp3,RealData)==0)
                {
                    sensorSInfo.temp3=JS(WD_BD_Addr3,sensorReal.temp3);
                    result=true;
                }
            }
            break;
        case 1://���궨
            break;
        case 2://���
            //���PH32K�궨
            ClearBD(WD_BD_Addr3);
            sensorSInfo.temp3=JS(WD_BD_Addr3,sensorReal.temp3);
            result=true;
            break;
        default:
            break;
        }

    }

    p1=strstr((const char*)p, (const char*)"\"ch3\"");//����ch3��ʶ
    p2=(char *)GetStringMiddle((uint8_t *)p1, len, ':', 1, ',', 1, &len1);
    if (p2==NULL) p2=(char *)GetStringMiddle((uint8_t *)p1, len+2, ':', 1, '}', 1, &len1);
    if (p2!=NULL)
    {
        sscanf(p2,"%d",&BDtype);//�õ���ǰPH35��������
        switch(BDtype)
        {
        case 0://�궨
            p1=strstr((const char*)p, (const char*)"\"h3\"");//����PH35ֵ��ʶ
            p2=(char *)GetStringMiddle((uint8_t *)p1, len, ':', 1, ',', 1, &len1);
            if (p2==NULL) p2=(char *)GetStringMiddle((uint8_t *)p1, len+2, ':', 1, '}', 1, &len1);
            if (p2!=NULL)
            {
                sscanf(p2,"%f",&RealData);//�õ���ǰPH35ֵ
                if (AddBD(SD_BD_Addr3,sensorReal.humi3,RealData)==0)
                {
                    sensorSInfo.humi3=JS(SD_BD_Addr2,sensorReal.humi3);
                    result=true;
                }
            }
            break;
        case 1://���궨
            break;
        case 2://���
            //���PH35�궨
            ClearBD(SD_BD_Addr3);
            sensorSInfo.humi3=JS(SD_BD_Addr3,sensorReal.humi3);
            result=true;
            break;
        default:
            break;
        }

    }

    p1=strstr((const char*)p, (const char*)"\"cp\"");//������ѹ��ʶ
    p2=(char *)GetStringMiddle((uint8_t *)p1, len, ':', 1, ',', 1, &len1);
    if (p2==NULL) p2=(char *)GetStringMiddle((uint8_t *)p1, len+2, ':', 1, '}', 1, &len1);
    if (p2!=NULL)
    {
        sscanf(p2,"%d",&BDtype);//�õ���ǰ��ѹ��������
        switch(BDtype)
        {
        case 0://�궨
            p1=strstr((const char*)p, (const char*)"\"p\"");//������ѹֵ��ʶ
            p2=(char *)GetStringMiddle((uint8_t *)p1, len, ':', 1, ',', 1, &len1);
            if (p2==NULL) p2=(char *)GetStringMiddle((uint8_t *)p1, len+2, ':', 1, '}', 1, &len1);
            if (p2!=NULL)
            {
                sscanf(p2,"%f",&RealData);//�õ���ǰ��ѹֵ
                if (AddBD(QY_BD_Addr,sensorReal.press,RealData)==0)
                {
                    sensorSInfo.press=JS(QY_BD_Addr,sensorReal.press);
                    result=true;
                }
            }
            break;
        case 1://���궨
            break;
        case 2://���
            //�����ѹ�궨
            ClearBD(QY_BD_Addr);
            sensorSInfo.press=JS(QY_BD_Addr,sensorReal.press);
            result=true;
            break;
        default:
            break;
        }

    }

    p1=strstr((const char*)p, (const char*)"\"cm\"");//����ˮ�ݱ�ʶ
    p2=(char *)GetStringMiddle((uint8_t *)p1, len, ':', 1, ',', 1, &len1);
    if (p2==NULL) p2=(char *)GetStringMiddle((uint8_t *)p1, len+2, ':', 1, '}', 1, &len1);
    if (p2!=NULL)
    {
        sscanf(p2,"%d",&BDtype);//�õ���ǰˮ�ݿ�������
        switch(BDtype)
        {
        case 0://�궨
            p1=strstr((const char*)p, (const char*)"\"m\"");//����ˮ��ֵ��ʶ
            p2=(char *)GetStringMiddle((uint8_t *)p1, len, ':', 1, ',', 1, &len1);
            if (p2==NULL) p2=(char *)GetStringMiddle((uint8_t *)p1, len+2, ':', 1, '}', 1, &len1);
            if (p2!=NULL)
            {
                sscanf(p2,"%f",&RealData);//�õ���ǰˮ��ֵ
                if (AddBD(SF_BD_Addr,sensorReal.moisture,RealData)==0)
                {
                    sensorSInfo.moisture=JS(SF_BD_Addr,sensorReal.moisture);
                    result=true;
                }
            }
            break;
        case 1://���궨
            break;
        case 2://���
            //���ˮ�ݱ궨
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
        p1=strstr((const char*)p, (const char*)"\"SN\"");//������ѹ��ʶ
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

    p1=strstr((const char*)p, (const char*)"\"AE\"");//����APPEUI��ʶ
    p2=(char *)GetStringMiddle((uint8_t *)p1, len, ':', 1, ',', 1, &len1);
    if (p2==NULL) p2=(char *)GetStringMiddle((uint8_t *)p1, len+2, ':', 1, '}', 1, &len1);
    if (p2!=NULL)
    {
        memcpy(LoRaNode.AppEUI,p2+1,APPEUI_Len);
        LoRaNode.AppEUI[APPEUI_Len]=0;
        Write_APPEUI_APPKEY((char *)LoRaNode.AppEUI,(char *)LoRaNode.AppKEY);
    }
    p1=strstr((const char*)p, (const char*)"\"AK\"");//����APPKEY��ʶ
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

    sensorSInfo.temp1=JS(WD_BD_Addr1,sensorReal.temp1);//���¼����¶�
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

