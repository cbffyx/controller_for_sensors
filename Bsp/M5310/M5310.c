#include "M5310.h"
#include "delay.h"
#include "string.h"
#include "SoftwareReset.h"
#include "protocal.h"

char *strx,*extstrx;
extern uint8_t FAC_SN[6];
extern char  usart3RxBuffer[];
extern uint16_t usart3RxCnt;
static uint8_t netOffLineCnt,IMEI[16];
u8 RecLen;
static M5310 M5310_Status;
static uint8_t err_cnt;
unsigned char udpRcvBuffer[usart3RxBufferSize];
unsigned char updataBuff[UPDATE_DOWN_REQ_LEN] = {0};

extern uint8_t ServerIP1[16];//����������IP
extern uint8_t ServerPort1[6];//�����������˿�
extern uint8_t ServerIP2[16];//��ʽ������IP
extern uint8_t ServerPort2[6];//��ʽ�������˿�

void setID(uint8_t* id)
{
	
    memset(id, 0, 16);
		memcpy(id,FAC_SN,6);
		memcpy(&id[6],IdF,3);
		memcpy(&id[9],&IMEI[9],7);
	
	//memcpy(id,"190729WM-7005909",16);
}

/*
�������ܣ������鳤��strlen��Χ�в���ƥ��ָ����ʼ�ַ���ָ�������ַ�֮������鷵�ظ�
������ڣ�
uint8_t *str ��Ҫ���ҵ�����
uint16_t strlen�����ҷ�Χ ָ�����ȷ�Χ
char str1����ʼ�ַ�
uint16_t Nostr1���ڼ�����ʼ�ַ�
char str2�������ַ�
uint16_t Nostr1���ڼ��������ַ�
uint16_t *substrlen�����ز��ҽ���ĳ���
���أ�����ҵ����򷵻ز��ҵ�λ�õ���ʼָ�룬δ�ҵ�������NULL
*/
uint8_t *GetStringMiddle(uint8_t *str, uint16_t strlen,
                         char str1, uint16_t Nostr1,
                         char str2, uint16_t Nostr2,
                         uint16_t *substrlen)
{
    uint16_t No1p;
    uint16_t No2p;
    uint16_t i;
    uint8_t *Firststr = NULL;
    uint8_t *Secstr = NULL;
    uint8_t Str1Ok;
    uint8_t Str2Ok;

    if(strlen==0)
    {
        return 0;
    }

    No1p=0;
    No2p=0;
    Str1Ok=0;
    Str2Ok=0;

    for (i=0; i<strlen; i++)
    {
        if (*(str+i)==str1)
        {
            No1p++;
            if (No1p==Nostr1)
            {
                Firststr=str+i;
                Str1Ok=1;
            }
        }

        if (*(str+i)==str2)
        {
            No2p++;
            if (No2p==Nostr2)
            {
                Secstr=str+i;
                Str2Ok=1;
            }
        }
        if (Str1Ok && Str2Ok)
        {
            break;  //��β�ַ����ҵ�
        }
    }

    if (Str1Ok&&Str2Ok)
    {
        if ((Secstr-Firststr)>=0)
        {
            *substrlen=Secstr-Firststr-1;
            return Firststr+1;
        }
        else
        {
            *substrlen=Firststr-Secstr-1;
            return Secstr+1;
        }
    }
    else
    {
        return 0;
    }
}

/*******************************************************************************
* @Function Name	: ģ��ʹ��
* @Description	:  .
* @Input		: None
* @Output	: None
* @Return	: None
*******************************************************************************/
void Enable_M5310(void)
{
    GPIO_InitTypeDef    GPIO_InitStructure;
    //оƬʹ�ܳ�ʼ��
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//push-pull
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//
    GPIO_Init(GPIOB,&GPIO_InitStructure);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
    GPIO_SetBits(GPIOB,GPIO_Pin_15);//��λ������
    delay_ms(500);

    GPIO_ResetBits(GPIOB,GPIO_Pin_13);//�������ͣ���λ������
}
void AtCmd(void)
{
    usart3RxCnt=0;
    do
    {
        usart3RxCnt=0;
        Uart1_SendStr("Send: AT\r\n");
        printf("AT\r\n");
        delay_ms(300);
        strx=strstr((const char*)usart3RxBuffer,(const char*)"OK");//����OK
        PrintUsart3Buffer();
    } while(strx==NULL);
    PrintClrUsart3Buffer();
}

uint8_t getCSQ(void)
{
    do
    {
        usart3RxCnt=0;
        Uart1_SendStr("Send: AT+CSQ\r\n");
        printf("AT+CSQ\r\n");//�鿴��ȡCSQֵ
        delay_ms(300);
        strx=strstr((const char*)usart3RxBuffer,(const char*)"+CSQ");//
        PrintUsart3Buffer();
        if(strx)
        {
            break;
        }
    } while(strx==NULL);
    M5310_Status.CSQ = (strx[5]-0x30)*10+(strx[6]-0x30);
    if(M5310_Status.CSQ==99)//˵��ɨ��ʧ��
    {
        Uart1_SendStr("�ź�����ʧ�ܣ���鿴ԭ��!\r\n");
    }
    PrintClrUsart3Buffer();
    return M5310_Status.CSQ;//��ȡCSQ
}

void getCEREG(void)
{
    do
    {
        usart3RxCnt=0;
        Uart1_SendStr("Send: AT+CEREG=1\r\n");
        printf("AT+CEREG=1\r\n");//�ж���Ӫ��
        delay_ms(300);
        strx=strstr((const char*)usart3RxBuffer,(const char*)"OK");//����OK
        PrintUsart3Buffer();
    } while(strx==NULL&&extstrx==NULL);

    PrintClrUsart3Buffer();

    do
    {
        usart3RxCnt=0;
        Uart1_SendStr("Send: AT+CEREG?\r\n");
        printf("AT+CEREG?\r\n");//�ж���Ӫ��
        delay_ms(300);
        strx=strstr((const char*)usart3RxBuffer,(const char*)"+CEREG:0,1");//����ע��״̬
        extstrx=strstr((const char*)usart3RxBuffer,(const char*)"+CEREG:1,1");//����ע��״̬
        PrintUsart3Buffer();
    } while(strx==NULL&&extstrx==NULL);
    PrintClrUsart3Buffer();
}

void getNUESTATS(void)
{
    do
    {
        usart3RxCnt=0;
        Uart1_SendStr("Send: AT+NUESTATS\r\n");
        printf("AT+NUESTATS\r\n");//
        delay_ms(300);
        strx=strstr((const char*)usart3RxBuffer,(const char*)"Signal");//����OK
        PrintUsart3Buffer();
    } while(strx==NULL&&extstrx==NULL);
    PrintClrUsart3Buffer();
}

void Connect(void)
{
    getCSQ();//
    getCEREG();
}

void IncNetOffLineCnt(void)
{
    netOffLineCnt++;
}
void ResetNetOffLineCnt(void)
{
    netOffLineCnt=0;
    err_cnt=0;
}
/*******************************************************************************
* @Function Name	:��������
* @Description	:����������򷵻���������������������.����������ʧ�ܣ���λģ�飬��������
* @Input		: None
* @Output	: None
* @Return	: None
*******************************************************************************/
uint8_t Reconnect(void)
{
    uint8_t i;
    i=0;
    AtCmd();
    usart3RxCnt=0;
    if(netOffLineCnt>=8)
    {
        Enable_M5310();
        delay_ms(300);
        AtCmd();
        usart3RxCnt=0;
        Uart1_SendStr("Send: AT+NRB\r\n");
        printf("AT+NRB\r\n");//����Hisi
        delay_second(10);
        AtCmd();
        err_cnt++;

        if(err_cnt>=6)  //(1080/wakeup_times) && wakeup_times<1080)
        {
            Uart1_SendStr("������λ������\r\n");
            Software_Reset();
        }
    }
    do
    {
        usart3RxCnt=0;
        Uart1_SendStr("Send: AT+CSQ\r\n");
        printf("AT+CSQ\r\n");//�鿴��ȡCSQֵ
        delay_ms(300);
        strx=strstr((const char*)usart3RxBuffer,(const char*)"+CSQ");//
        PrintUsart3Buffer();
        if(strx)
        {
            break;
        }
    } while(strx==NULL);
    do
    {
        usart3RxCnt=0;
        Uart1_SendStr("Send: AT+CGATT?\r\n");
        printf("AT+CGATT?\r\n");//��ȡ����״̬
        delay_ms(300);
        strx=strstr((const char*)usart3RxBuffer,(const char*)"+CGATT:1");//����1,����ע���ɹ�
        extstrx=strstr((const char*)usart3RxBuffer,(const char*)"+CGATT:0");//û�м������� ��0
        PrintUsart3Buffer();
    } while(strx==NULL&&extstrx==NULL);
    PrintClrUsart3Buffer();
/*
    do
    {
        usart3RxCnt=0;
        Uart1_SendStr("Send: AT+CGATT=0\r\n");
        printf("AT+CGATT=0\r\n");//ȥ����
        delay_ms(10000);
        //strx=strstr((const char*)usart3RxBuffer,(const char*)"+CGATT:1");//����1,����ע���ɹ�
        //extstrx=strstr((const char*)usart3RxBuffer,(const char*)"+CGATT:0");//û�м������� ��0
				strx=strstr((const char*)usart3RxBuffer,(const char*)"OK");//����OK,�����ɹ�
        PrintUsart3Buffer();
    } while(strx==NULL&&extstrx==NULL);
    PrintClrUsart3Buffer();
*/
    if(strx!=NULL) //��������������  CGATT:1
    {
        return 0;
    }
    if(extstrx!=NULL) //CGATT:0
    {
        usart3RxCnt=0;
        Uart1_SendStr("Send: AT+CGATT=1\r\n");
        printf("AT+CGATT=1\r\n");//�������磬PDP
        delay_ms(300);
        M5310_backok();
        do
        {
            i++;
            if (i==200)
            {
                break;
            }
            usart3RxCnt=0;
            Uart1_SendStr("Send: AT+CGATT?\r\n");
            printf("AT+CGATT?\r\n");//��ȡ����״̬
            delay_ms(300);
            strx=strstr((const char*)usart3RxBuffer,(const char*)"+CGATT:1");//����1,����ע���ɹ�
            PrintUsart3Buffer();
        } while(strx==NULL);
        PrintClrUsart3Buffer();

        if(i<200)
        {
            getNUESTATS();
            Connect();
            netOffLineCnt=0;
            return 0;
        }
        else
        {
            getNUESTATS();
            netOffLineCnt++;
            return 1;
        }
    }
    netOffLineCnt++;
    return 1;
}

void ClosePSMeDRX(void)
{
    usart3RxCnt=0;
    Uart1_SendStr("Send: AT+CPSMS=0\r\n");
    printf("AT+CPSMS=0\r\n");//�ر�PSM
    M5310_backok();

    usart3RxCnt=0;
    Uart1_SendStr("Send: AT+CEDRXS=0\r\n");
    printf("AT+CEDRXS=0\r\n");//�ر�eDRX
    M5310_backok();
}
void SetPSMeDRX(void)
{
    usart3RxCnt=0;
    Uart1_SendStr("Send: AT+CPSMS=1,,,00001001,00011001\r\n");
    printf("AT+CPSMS=1,,,00001001,00011001\r\n");
    // 000 10min*01001=90min,
    // 001 1hour*00001=1hour,000 2sec*11001=25sec
    delay_ms(300);
    //M5310_backok();
    Uart1_SendStr("PSM 20s,1hour����\r\n");
}
/*******************************************************************************
* @Function Name	: ģ���ʼ��
* @Description	:  .
* @Input		: None
* @Output	: None
* @Return	: None
*******************************************************************************/
void M5310_Init(void)
{
    uint8_t i;

    err_cnt=0;
    Enable_M5310();
    AtCmd();
    usart3RxCnt=0;
    Uart1_SendStr("Send: AT+NRB\r\n");
    printf("AT+NRB\r\n");//����Hisi
    delay_ms(300);
    AtCmd();
    SetPSMeDRX();

    usart3RxCnt=0;
    Uart1_SendStr("Send: AT+CMEE=1\r\n");
    printf("AT+CMEE=1\r\n"); //��������ֵ
    delay_ms(300);
    strx=strstr((const char*)usart3RxBuffer,(const char*)"OK");//����OK
    PrintUsart3Buffer();

    do
    {
        usart3RxCnt=0;
        Uart1_SendStr("Send: AT+NBAND?\r\n");
        printf("AT+NBAND?\r\n");//��ȡƵ�κ�
        delay_ms(300);
        strx=strstr((const char*)usart3RxBuffer,(const char*)"+NBAND:8");//����OK
        PrintUsart3Buffer();
    } while(strx==NULL);
    PrintClrUsart3Buffer();

    do
    {
        usart3RxCnt=0;
        Uart1_SendStr("Send: AT+CIMI\r\n");
        printf("AT+CIMI\r\n");//��ȡ���ţ������Ƿ���ڿ�����˼���Ƚ���Ҫ��
        delay_ms(300);
        strx=strstr((const char*)usart3RxBuffer,(const char*)"4600");//����OK,˵�����Ǵ��ڵ�
        PrintUsart3Buffer();
        if(strx!=NULL)
        {   IMEI[0]=0x30;
            for(i=0; i<15; i++) {
                IMEI[i+1]=strx[i];
            }
        }
    } while(strx==NULL);
    PrintClrUsart3Buffer();
    Reconnect();

}
/*******************************************************************************
* @Function Name	: ��һ���׼���
* @Description	:  .
* @Input		: None
* @Output	: None
* @Return	: None
*******************************************************************************/
void M5310_ConUDP(void)
{
    usart3RxCnt=0;
    Uart1_SendStr("Send: AT+NSOCL=0\r\n");
    printf("AT+NSOCL=0\r\n");//�ر�socekt����
    delay_ms(300);
    M5310_backok();
    usart3RxCnt=0;
    Uart1_SendStr("Send: AT+NSOCR=DGRAM,17,2454,1\r\n");
    printf("AT+NSOCR=DGRAM,17,2454,1\r\n");//����һ��Socket
    delay_ms(300);
    M5310_backok();
}
/*******************************************************************************
* @Function Name	:  �ر��׼���
* @Description	:  .
* @Input		: None
* @Output	: None
* @Return	: None
*******************************************************************************/
void M5310_DisconUDP(void)
{
    usart3RxCnt=0;
    Uart1_SendStr("Send: AT+NSOCL=0\r\n");
    printf("AT+NSOCL=0\r\n");//�ر�socekt����
    M5310_backok();

}
/*******************************************************************************
* @Function Name	:�������ݺ��� �����ӣ�
* @Description	:  .
* @Input		: ����+����
* @Output	: None
* @Return	: None
*******************************************************************************/
void M5310_Senddata(uint8_t len,uint8_t *data)
{
    char str[100];
    usart3RxCnt=0;
    sprintf(str,"Send: AT+NSOST=0,%s,%s,%c,%s\r\n",ServerIP2,ServerPort2,'8',"727394ACB8221234");
    Uart1_SendStr(str);
    printf("AT+NSOST=0,%s,%s,%c,%s\r\n",ServerIP2,ServerPort2,'8',"727394ACB8221234");//����0socketIP�Ͷ˿ں������Ӧ���ݳ����Լ�����,
    delay_ms(300);
    M5310_backok();
}
/*******************************************************************************
* @Function Name	:�������ݺ���
* @Description	:���յ�����������������ַ�����ANSIת��.
* @Input		: None
* @Output	: None
* @Return	: None
*******************************************************************************/
void M5310UdpRcvHandler(uint16_t DelyTime,uint8_t mess_type,uint8_t IPType)
{
    uint16_t i = 0;
    uint16_t subLen = 0;
    uint8_t subStr[5] = {0};
    char str[256];

    do
    {
        i++;
        if(i==20) {
            Uart1_SendStr("RecvTimeOut! Reconnect...");
            M5310_ConUDP();
            return;
        }
        strx=strstr((const char*)usart3RxBuffer,(const char*)"+NSONMI:");//����"+NSONMI:0,290"���������յ�UDP���������ص�����
        delay_ms(500);
    } while(strx==NULL);
    PrintUsart3Buffer();
    M5310_Status.Socketnum = strx[8];
    strx = (char *)GetStringMiddle((uint8_t *)strx, 15, ',', 1, '\r', 1, &subLen);
    if(subLen > 4)
    {
        subLen = 4;
    }
    memcpy(subStr, strx, subLen);
    subStr[subLen] = '\0';
    usart3RxCnt=0;
    sprintf(str,"Send: AT+NSORF=%c,%s\r\n",M5310_Status.Socketnum, subStr);
    Uart1_SendStr(str);
    printf("AT+NSORF=%c,%s\r\n",M5310_Status.Socketnum, subStr);//��ȡ����
    delay_ms(DelyTime);
    //PrintUsart3Buffer();
    strx=strstr((const char*)usart3RxBuffer,(const char*)",");//��ȡ����һ������
    strx=strstr((const char*)(strx+1),(const char*)",");//��ȡ���ڶ�������
    strx=strstr((const char*)(strx+1),(const char*)",");//��ȡ������������
    M5310_Status.recdatalen= 0;
    for(i=1;; i++) {
        if(strx[i]==',')
            break;
        if (i>6) break;
        if ((strx[i]<'0')||(strx[i]>'9')) break;
        M5310_Status.recdatalen=	M5310_Status.recdatalen*10+strx[i]-'0'	;
    }
		if ((mess_type==MSG_TYPE_SAMPLE_TIME)||(mess_type==MSG_TYPE_UPDATE_DATA)||(mess_type==MSG_TYPE_CALIB_NONGDU)||(mess_type==MSG_TYPE_CALIB_TIME))
		{
			i=M5310_Status.recdatalen*2+strlen((const char*)ServerIP1)+strlen((const char*)ServerPort1)+sizeof(subStr)+12;
		}
		else
		{
			i=M5310_Status.recdatalen*2+strlen((const char*)ServerIP2)+strlen((const char*)ServerPort2)+sizeof(subStr)+12;
		}
    //sprintf(str, "Uart1_SendStr:%d, i:%d\r\n", usart3RxCnt, i);
    //Uart1_SendStr(str);
    if (usart3RxCnt < i )
    {
        usart3RxCnt=0;
        Uart1_SendStr("RecvLen Error!");
        return;
    }
    if (M5310_Status.recdatalen>3000)
    {
        usart3RxCnt=0;
        Uart1_SendStr("RecvLen Out 3000!");
        return;
    }
    strx=strstr((const char*)(strx+1),(const char*)",");//��ȡ�����ĸ�����
    strx++;

    //��ʽ������
    for(i=0; i<M5310_Status.recdatalen*2;) {
        u8 l1;
        u8 l2;
        if(strx[i]<='9'&&strx[i]>='0') {
            l1=strx[i]-'0';
        }
        else if(strx[i]<='f'&&strx[i]>='a') {
            l1=strx[i]-'a'+10;
        }
        else if(strx[i]<='F'&&strx[i]>='A') {
            l1=strx[i]-'A'+10;
        }

        if(strx[i+1]<='9'&&strx[i+1]>='0') {
            l2=strx[i+1]-'0';
        }
        else if(strx[i+1]<='f'&&strx[i+1]>='a') {
            l2=strx[i+1]-'a'+10;
        }
        else if(strx[i+1]<='F'&&strx[i+1]>='A') {
            l2=strx[i+1]-'A'+10;
        }
        udpRcvBuffer[i/2]=l1*16+l2;
        i=i+2;
    }
    //PrintUsart3Buffer();
    RecvServerPackage(M5310_Status.recdatalen, udpRcvBuffer,IPType);
    PrintClrUsart3Buffer();
}


void M5310_backok(void)
{
    uint16_t i = 0;
    do
    {
        i++;
        if(i==20) return;
        strx=strstr((const char*)usart3RxBuffer,(const char*)"OK");//����OK
        extstrx=strstr((const char*)usart3RxBuffer,(const char*)"ERR");//����
        delay_ms(200);
    } while(strx==NULL&&extstrx==NULL);

    PrintUsart3Buffer();
}

