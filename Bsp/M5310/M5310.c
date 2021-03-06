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

extern uint8_t ServerIP1[16];//生产服务器IP
extern uint8_t ServerPort1[6];//生产服务器端口
extern uint8_t ServerIP2[16];//正式服务器IP
extern uint8_t ServerPort2[6];//正式服务器端口

void setID(uint8_t* id)
{
	
    memset(id, 0, 16);
		memcpy(id,FAC_SN,6);
		memcpy(&id[6],IdF,3);
		memcpy(&id[9],&IMEI[9],7);
	
	//memcpy(id,"190729WM-7005909",16);
}

/*
函数功能：在数组长度strlen范围中查找匹配指定开始字符和指定结束字符之间的数组返回给
函数入口：
uint8_t *str ：要查找的数组
uint16_t strlen：查找范围 指定长度范围
char str1：开始字符
uint16_t Nostr1：第几个开始字符
char str2：结束字符
uint16_t Nostr1：第几个结束字符
uint16_t *substrlen：传回查找结果的长度
返回：如果找到，则返回查找到位置的起始指针，未找到，返回NULL
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
            break;  //锟斤拷尾锟街凤拷锟斤拷锟揭碉拷
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
* @Function Name	: 模组使能
* @Description	:  .
* @Input		: None
* @Output	: None
* @Return	: None
*******************************************************************************/
void Enable_M5310(void)
{
    GPIO_InitTypeDef    GPIO_InitStructure;
    //芯片使能初始化
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//push-pull
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//
    GPIO_Init(GPIOB,&GPIO_InitStructure);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
    GPIO_SetBits(GPIOB,GPIO_Pin_15);//复位则拉高
    delay_ms(500);

    GPIO_ResetBits(GPIOB,GPIO_Pin_13);//引脚拉低，复位则拉高
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
        strx=strstr((const char*)usart3RxBuffer,(const char*)"OK");//返回OK
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
        printf("AT+CSQ\r\n");//查看获取CSQ值
        delay_ms(300);
        strx=strstr((const char*)usart3RxBuffer,(const char*)"+CSQ");//
        PrintUsart3Buffer();
        if(strx)
        {
            break;
        }
    } while(strx==NULL);
    M5310_Status.CSQ = (strx[5]-0x30)*10+(strx[6]-0x30);
    if(M5310_Status.CSQ==99)//说明扫网失败
    {
        Uart1_SendStr("信号搜索失败，请查看原因!\r\n");
    }
    PrintClrUsart3Buffer();
    return M5310_Status.CSQ;//获取CSQ
}

void getCEREG(void)
{
    do
    {
        usart3RxCnt=0;
        Uart1_SendStr("Send: AT+CEREG=1\r\n");
        printf("AT+CEREG=1\r\n");//判断运营商
        delay_ms(300);
        strx=strstr((const char*)usart3RxBuffer,(const char*)"OK");//返回OK
        PrintUsart3Buffer();
    } while(strx==NULL&&extstrx==NULL);

    PrintClrUsart3Buffer();

    do
    {
        usart3RxCnt=0;
        Uart1_SendStr("Send: AT+CEREG?\r\n");
        printf("AT+CEREG?\r\n");//判断运营商
        delay_ms(300);
        strx=strstr((const char*)usart3RxBuffer,(const char*)"+CEREG:0,1");//返回注册状态
        extstrx=strstr((const char*)usart3RxBuffer,(const char*)"+CEREG:1,1");//返回注册状态
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
        strx=strstr((const char*)usart3RxBuffer,(const char*)"Signal");//返回OK
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
* @Function Name	:重新连接
* @Description	:如果已连接则返回连接正常，否则尝试连接.如果多次连接失败，则复位模组，重新配置
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
        printf("AT+NRB\r\n");//重启Hisi
        delay_second(10);
        AtCmd();
        err_cnt++;

        if(err_cnt>=6)  //(1080/wakeup_times) && wakeup_times<1080)
        {
            Uart1_SendStr("软件复位！！！\r\n");
            Software_Reset();
        }
    }
    do
    {
        usart3RxCnt=0;
        Uart1_SendStr("Send: AT+CSQ\r\n");
        printf("AT+CSQ\r\n");//查看获取CSQ值
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
        printf("AT+CGATT?\r\n");//获取激活状态
        delay_ms(300);
        strx=strstr((const char*)usart3RxBuffer,(const char*)"+CGATT:1");//返回1,表明注网成功
        extstrx=strstr((const char*)usart3RxBuffer,(const char*)"+CGATT:0");//没有激活网络 返0
        PrintUsart3Buffer();
    } while(strx==NULL&&extstrx==NULL);
    PrintClrUsart3Buffer();
/*
    do
    {
        usart3RxCnt=0;
        Uart1_SendStr("Send: AT+CGATT=0\r\n");
        printf("AT+CGATT=0\r\n");//去附着
        delay_ms(10000);
        //strx=strstr((const char*)usart3RxBuffer,(const char*)"+CGATT:1");//返回1,表明注网成功
        //extstrx=strstr((const char*)usart3RxBuffer,(const char*)"+CGATT:0");//没有激活网络 返0
				strx=strstr((const char*)usart3RxBuffer,(const char*)"OK");//返回OK,表明成功
        PrintUsart3Buffer();
    } while(strx==NULL&&extstrx==NULL);
    PrintClrUsart3Buffer();
*/
    if(strx!=NULL) //在网，无需重连  CGATT:1
    {
        return 0;
    }
    if(extstrx!=NULL) //CGATT:0
    {
        usart3RxCnt=0;
        Uart1_SendStr("Send: AT+CGATT=1\r\n");
        printf("AT+CGATT=1\r\n");//激活网络，PDP
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
            printf("AT+CGATT?\r\n");//获取激活状态
            delay_ms(300);
            strx=strstr((const char*)usart3RxBuffer,(const char*)"+CGATT:1");//返回1,表明注网成功
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
    printf("AT+CPSMS=0\r\n");//关闭PSM
    M5310_backok();

    usart3RxCnt=0;
    Uart1_SendStr("Send: AT+CEDRXS=0\r\n");
    printf("AT+CEDRXS=0\r\n");//关闭eDRX
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
    Uart1_SendStr("PSM 20s,1hour配置\r\n");
}
/*******************************************************************************
* @Function Name	: 模组初始化
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
    printf("AT+NRB\r\n");//重启Hisi
    delay_ms(300);
    AtCmd();
    SetPSMeDRX();

    usart3RxCnt=0;
    Uart1_SendStr("Send: AT+CMEE=1\r\n");
    printf("AT+CMEE=1\r\n"); //允许错误值
    delay_ms(300);
    strx=strstr((const char*)usart3RxBuffer,(const char*)"OK");//返回OK
    PrintUsart3Buffer();

    do
    {
        usart3RxCnt=0;
        Uart1_SendStr("Send: AT+NBAND?\r\n");
        printf("AT+NBAND?\r\n");//获取频段号
        delay_ms(300);
        strx=strstr((const char*)usart3RxBuffer,(const char*)"+NBAND:8");//返回OK
        PrintUsart3Buffer();
    } while(strx==NULL);
    PrintClrUsart3Buffer();

    do
    {
        usart3RxCnt=0;
        Uart1_SendStr("Send: AT+CIMI\r\n");
        printf("AT+CIMI\r\n");//获取卡号，类似是否存在卡的意思，比较重要。
        delay_ms(300);
        strx=strstr((const char*)usart3RxBuffer,(const char*)"4600");//返回OK,说明卡是存在的
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
* @Function Name	: 打开一个套间字
* @Description	:  .
* @Input		: None
* @Output	: None
* @Return	: None
*******************************************************************************/
void M5310_ConUDP(void)
{
    usart3RxCnt=0;
    Uart1_SendStr("Send: AT+NSOCL=0\r\n");
    printf("AT+NSOCL=0\r\n");//关闭socekt连接
    delay_ms(300);
    M5310_backok();
    usart3RxCnt=0;
    Uart1_SendStr("Send: AT+NSOCR=DGRAM,17,2454,1\r\n");
    printf("AT+NSOCR=DGRAM,17,2454,1\r\n");//创建一个Socket
    delay_ms(300);
    M5310_backok();
}
/*******************************************************************************
* @Function Name	:  关闭套间字
* @Description	:  .
* @Input		: None
* @Output	: None
* @Return	: None
*******************************************************************************/
void M5310_DisconUDP(void)
{
    usart3RxCnt=0;
    Uart1_SendStr("Send: AT+NSOCL=0\r\n");
    printf("AT+NSOCL=0\r\n");//关闭socekt连接
    M5310_backok();

}
/*******************************************************************************
* @Function Name	:发送数据函数 （例子）
* @Description	:  .
* @Input		: 长度+数据
* @Output	: None
* @Return	: None
*******************************************************************************/
void M5310_Senddata(uint8_t len,uint8_t *data)
{
    char str[100];
    usart3RxCnt=0;
    sprintf(str,"Send: AT+NSOST=0,%s,%s,%c,%s\r\n",ServerIP2,ServerPort2,'8',"727394ACB8221234");
    Uart1_SendStr(str);
    printf("AT+NSOST=0,%s,%s,%c,%s\r\n",ServerIP2,ServerPort2,'8',"727394ACB8221234");//发送0socketIP和端口后面跟对应数据长度以及数据,
    delay_ms(300);
    M5310_backok();
}
/*******************************************************************************
* @Function Name	:接收数据函数
* @Description	:接收到数据则解析，并将字符按照ANSI转码.
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
        strx=strstr((const char*)usart3RxBuffer,(const char*)"+NSONMI:");//返回"+NSONMI:0,290"，表明接收到UDP服务器发回的数据
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
    printf("AT+NSORF=%c,%s\r\n",M5310_Status.Socketnum, subStr);//读取数据
    delay_ms(DelyTime);
    //PrintUsart3Buffer();
    strx=strstr((const char*)usart3RxBuffer,(const char*)",");//获取到第一个逗号
    strx=strstr((const char*)(strx+1),(const char*)",");//获取到第二个逗号
    strx=strstr((const char*)(strx+1),(const char*)",");//获取到第三个逗号
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
    strx=strstr((const char*)(strx+1),(const char*)",");//获取到第四个逗号
    strx++;

    //格式化数据
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
        strx=strstr((const char*)usart3RxBuffer,(const char*)"OK");//返回OK
        extstrx=strstr((const char*)usart3RxBuffer,(const char*)"ERR");//错误
        delay_ms(200);
    } while(strx==NULL&&extstrx==NULL);

    PrintUsart3Buffer();
}


