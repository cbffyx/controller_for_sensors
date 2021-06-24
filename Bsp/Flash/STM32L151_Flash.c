/**
  * @brief  EEPROM program.
  * @description ��EEPROM����Ҫ����һЩ����
  * @param  teper
  */

#include "STM32L151_Flash.h"
#include "usart.h"
#include "string.h"
#include "main.h"
#include "ProUart1.h"

#define MaxAddress 10 //10�����׸�����λ�ã�11�������ֵλ��

__IO FLASH_Status FLASHStatus;
__IO TestStatus DataMemoryProgramStatus;
uint32_t NbrOfPage, j, Address;
extern package send_package;

void flash_init(void){
//��������jicunq����
/* Unlock the FLASH PECR register and Data EEPROM memory */
  DATA_EEPROM_Unlock();
  
  /* Clear all pending flags */      
  FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR
                  | FLASH_FLAG_SIZERR | FLASH_FLAG_OPTVERR | FLASH_FLAG_OPTVERRUSR);	
}


float Uchar2Float(uint32_t Address)//����float
{
	return *(__IO float*)Address;
}

void Float2Uchar(uint32_t Address,float f)//д��float
{
	uint8_t i;
	for(i=0;i<4;i++)
	{
		DATA_EEPROM_FastProgramByte(Address+i,((uint8_t *)&f)[i]);	
	}	
}

void Setchar_cur(uint32_t number,uint8_t data)
{
	DATA_EEPROM_FastProgramByte(DATA_EEPROM_START_ADDR+number+ParamentOffset,data);
}
uint8_t getu8_cur(uint32_t cur)//����ڼ������õ���Ӧֵ
{
	return (*(__IO uint8_t*)(DATA_EEPROM_START_ADDR+cur+ParamentOffset));
}

char getchar_cur(uint32_t cur)//����ڼ������õ���Ӧֵ
{
	return (*(__IO char*)(DATA_EEPROM_START_ADDR+cur+ParamentOffset));
}

//���ÿ��
void erase_allword(void){
		flash_init();
/*  Data EEPROM Fast Word program of FAST_DATA_32 at addresses defined by 
      DATA_EEPROM_START_ADDR and DATA_EEPROM_END_ADDR */  
  Address = DATA_EEPROM_START_ADDR;

  NbrOfPage = ((DATA_EEPROM_END_ADDR - Address) + 1 ) >> 2; //��ȥ4
  
  /* Erase the Data EEPROM Memory pages by Word (32-bit) */
  for(j = 0; j < NbrOfPage; j++)
  {
    FLASHStatus = DATA_EEPROM_EraseWord(Address + (4 * j));
  }
   
  /* Check the correctness of written data */
  while(Address < DATA_EEPROM_END_ADDR)
  {
    if(*(__IO uint32_t*)Address != 0x0)
    {
      DataMemoryProgramStatus = FAILED;
    }
    Address = Address + 4;
  }
}


/*
void SetZeroVoltage(uint32_t data)//�洢�߾������
{
	DATA_EEPROM_ProgramWord(DATA_EEPROM_START_ADDR+360,data);
}
uint32_t GetZeroVoltage(void)
{
	return (*(__IO uint32_t*)(DATA_EEPROM_START_ADDR+360));
}

void SetVoltage(uint32_t number,uint32_t data)//�ڵڼ�����������
{
	DATA_EEPROM_ProgramWord(DATA_EEPROM_START_ADDR+number*12,data);
}

void SetConcentration(uint32_t number,uint32_t data)
{
	DATA_EEPROM_ProgramWord(DATA_EEPROM_START_ADDR+number*12+4,data);
}

void Setdata_cur(uint32_t number,uint32_t data)
{
	DATA_EEPROM_ProgramWord(DATA_EEPROM_START_ADDR+number*12+8,data);
}

uint32_t getVoltage(uint32_t cur)//����ڼ������õ���Ӧֵ
{
	return (*(__IO uint32_t*)(DATA_EEPROM_START_ADDR+cur*12));
}
uint32_t getConcentration(uint32_t cur)
{
	return (*(__IO uint32_t*)(DATA_EEPROM_START_ADDR+cur*12+4));
}
uint32_t getdata_cur(uint32_t cur)
{
	return (*(__IO uint32_t*)(DATA_EEPROM_START_ADDR+cur*12+8));
}

TestStatus InitList()//��ʼ��,��������ʹ��
{
	int i;
	flash_init();
	for(i=0;i<MaxAddress;i++)
		{
			Setdata_cur(i,i+1);
			SetVoltage(i,0);
			SetConcentration(i,0);
		}
	Setdata_cur(MaxAddress,0);
	return PASSED;
}

TestStatus ListInsert(uint32_t Voltage,uint32_t Concentration)
{
	uint16_t j,k,l;
	uint32_t littler,larger;
	littler=larger=0;
		flash_init();
	j = getdata_cur(MaxAddress);
	if(j<MaxAddress)
	{
		k=0;
		if(Concentration>0)
		{
			for(l=0;l<j;l++)
			{
				if(Concentration>getConcentration(k))
				littler=k;
				if(Concentration<getConcentration(k))
				{
					larger=k;
					break;
				}
				if(Concentration==getConcentration(k))
					return PASSED;
				k=getdata_cur(k);
				if(k==0) break;
			}
		}
		Setdata_cur(MaxAddress,getdata_cur(j));
		SetVoltage(j,Voltage);
		SetConcentration(j,Concentration);
		
		if(littler!=j)
			Setdata_cur(littler,j);
		if(larger>0)
			Setdata_cur(j,larger);
		else //if(j!=0)
		{
			Setdata_cur(j,0);//����β��
			Setdata_cur(MaxAddress+1,j);//��ʱjΪ���ֵ������11��
		}
		return PASSED;
	}
	return FAILED;
}
*/


/*******************************************************************************
* @Function Name	:Setter��Getter  
* @Description	:  .
* @Input		: None
* @Output	: None
* @Return	: None
*******************************************************************************/
void Set_IPPort1(char* ip,char* port)
{
	uint8_t i;
	flash_init();
	for(i=0;i<4;i++)
	{
		Setchar_cur(IP1_Addr+i,ip[i]);
	}
	for(i=0;i<2;i++)
	{
		Setchar_cur(Port1_Addr+i,port[i]);
	}
}

void Get_IPPort1(char* ip,char* port)//����������
{
	uint8_t i,ipis0,portis0;
	uint8_t tmpip[]=DServerIP1;
	ipis0=0;portis0=0;
	for(i=0;i<4;i++)
	{
		ip[i]=getu8_cur(IP1_Addr+i);
		if (ip[i]!= 0) ipis0=1;
	}
	for(i=0;i<2;i++)
	{
		port[i]=getu8_cur(Port1_Addr+i);
		if (port[i]!= 0) portis0=1;
	}
	if ((ipis0==0)||(portis0==0))
	{
		memcpy(ip,tmpip,4);
		port[0]=DServerPort1/256;
		port[1]=DServerPort1%256;
		Set_IPPort1(ip,port);
	}
}

void Set_IPPort2(char* ip,char* port)
{
	uint8_t i;
	flash_init();
	for(i=0;i<4;i++)
	{
		Setchar_cur(IP2_Addr+i,ip[i]);
	}
	for(i=0;i<2;i++)
	{
		Setchar_cur(Port2_Addr+i,port[i]);
	}
}
void Get_IPPort2(char* ip,char* port)
{
	uint8_t i,ipis0,portis0;
	uint8_t tmpip[]=DServerIP2;
	ipis0=0;portis0=0;
	for(i=0;i<4;i++)
	{
		ip[i]=getu8_cur(IP2_Addr+i);
		if (ip[i]!= 0) ipis0=1;
	}
	for(i=0;i<2;i++)
	{
		port[i]=getu8_cur(Port2_Addr+i);
		if (port[i]!= 0) portis0=1;
	}
	if ((ipis0==0)||(portis0==0))
	{
		memcpy(ip,tmpip,4);
		port[0]=DServerPort2/256;
		port[1]=DServerPort2%256;
		Set_IPPort2(ip,port);
	}
}

void Set_DeviceStatue(uint8_t stats)
{
	flash_init();
	Setchar_cur(DeviceStatues_Addr,stats);
}
uint8_t Get_DeviceStatue(void)
{
	uint8_t status;
	status=getu8_cur(DeviceStatues_Addr);
	if (status==0) 
	{
		status=1;//1��������
		Set_DeviceStatue(status);
	}
	return status;
}
void Set_RunStatue(uint8_t stats)
{
	flash_init();
	Setchar_cur(RunStatues_Addr,stats);
}
uint8_t Get_RunStatue(void)
{
	uint8_t status;
	status=getu8_cur(RunStatues_Addr);
	if (status==0) 
	{
		status=5;//5 δʹ��
		Set_RunStatue(status);
	}
	return status;
}

void Set_Fac_SN(char* SN)
{
	uint8_t i;
	flash_init();
	for(i=0;i<3;i++)
	{
		Setchar_cur(Fac_SN_Addr+i,SN[i]);
	}
}
void Get_Fac_SN(char* SN)
{
	uint8_t i;
	for(i=0;i<3;i++)
	{
		SN[i]=getu8_cur(Fac_SN_Addr+i);
	}
}

void Set_Dfrequency(uint16_t dat)
{
	uint8_t i,df[2];
	flash_init();
	*((uint16_t*)(df))=dat;
	for(i=0;i<2;i++)
	{
		Setchar_cur(Dfrequency_Addr+i,df[i]);
	}
}

void Get_Dfrequency(uint16_t *dat)
{
	uint8_t i,df[2];
	for(i=0;i<2;i++)
	{
		df[i]=getu8_cur(Dfrequency_Addr+i);
	}
	*dat=*((uint16_t*)(df));
}

/*
#define IP1_Addr 						0x00		//����������IP
#define Por1_Addr 					0x10		//�����������˿�
#define IP2_Addr 						0x20		//��ʽ������IP
#define Port2_Addr 					0x30		//��ʽ�������˿�
#define MotoStatue_Addr 		0x40		//���״̬
#define DeviceStatue_Addr 	0x48		//�豸״̬ 1��������2 ���� 3 ���궨 4����
#define RunStatue_Addr 			0x50		//����״̬ 1 Ѭ��׼�� 2 Ѭ���� 3ɢ���� 4 Ѭ������ 5δʹ��
#define Fac_SN_Addr					0x60		//�������κ�
#define CT_Addr							0x70		//�¶�У׼������ϵ����
#define CH_Addr							0x80		//ʪ��У׼������ϵ����
#define CPH32k_Addr					0x90		//PH3 2000������У׼������ϵ����
#define CPH35_Addr					0xA0		//PH3 5 ������У׼������ϵ����
*/
