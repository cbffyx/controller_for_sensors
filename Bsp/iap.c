#include "iap.h"
#include "stdio.h"
#include <stdbool.h>
#include "w25qx.h"
#include "memMid.h"
#include "memMgr.h"
#include "string.h"

extern uint8_t w25qx_buffer[];
extern void Uart1_SendStr(char*SendBuf);
extern void UART1_send_byte(char data);
IapHeaderCrcDef iapHeadCrc1;

uint8_t CountMod256(uint8_t *data, uint16_t len)
{
	uint16_t i = 0;
	uint8_t sum = 0;
	
	for(i=0; i<len; i++)
	{
		sum += *data;
		data++;
	}

	return sum;
}

void init_crc_table(uint32_t *table)
{
	unsigned int c;
	unsigned int i, j;

	for (i = 0; i < 256; i++) {
		c = (unsigned int)i;
		for (j = 0; j < 8; j++) {
			if (c & 1)
				c = 0xedb88320 ^ (c >> 1);
			else
				c = c >> 1;
		}
		table[i] = c;
	}
}

/*计算buffer的crc校验码*/
unsigned long Crc32(uint32_t *crc_table, unsigned long crc,unsigned char *buffer, unsigned int size)
{

	unsigned int i;
	for (i = 0; i < size; i++) {
		crc = (crc >> 8) ^ crc_table[(crc & 0xFF) ^ buffer[i]];
		//crc = crc_table[(crc ^ buffer[i]) & 0xff] ^ (crc >> 8);
		//crc = Crc32Table[(crc ^ buffer[i]) & 0xff] ^ (crc >> 8);
	}
	return crc ;
}


bool IsMemValid(uint8_t *record, uint16_t recordLen, RecordCkDef ck)
{
	uint8_t sum = 0;
	
	if(ck.isValid != RECORD_VALID_FLAG)
	{
		return false;
	}
	sum = CountMod256(record, recordLen);
	if(sum != ck.ckSum)
	{
		return false;
	}
	return true;
}

void CountMemCk(uint8_t *record, uint16_t recordLen, RecordCkDef *ck)
{
	ck->ckSum = CountMod256(record, recordLen);
	ck->isValid= RECORD_VALID_FLAG;
}

void IapHeadCrc1ReInit( void )
{
	
}

void ReadIapHeadCrc1(void)
{
	bool isValid = false;
	if( 0 != Mem_Read(APP_FILE_INFO_ADDR, (uint8_t*)&iapHeadCrc1, sizeof(iapHeadCrc1)) )
	{
		IapHeadCrc1ReInit();
		return;
	}

	isValid = IsMemValid((uint8_t *)&iapHeadCrc1.data, sizeof(iapHeadCrc1.data), iapHeadCrc1.ck);
	if(!isValid)
	{
		IapHeadCrc1ReInit();
		return;
	}
}

void WriteIapHeadCrc1(void)
{
	Mem_Erase( APP_FILE_INFO_ADDR, W25Q128FV_SUBSECTOR_SIZE);
	CountMemCk((uint8_t *)&iapHeadCrc1.data, sizeof(iapHeadCrc1.data), &iapHeadCrc1.ck);
	Mem_Write_Ex( APP_FILE_INFO_ADDR, (uint8_t*)&iapHeadCrc1, sizeof(iapHeadCrc1));
}

bool CheckIap(void)
{
	uint32_t offset = 0;
	uint16_t len = 0; 
	uint32_t crc32 = 0xffffffff;
	uint32_t crc_table[256] = {0};
	char str[50];
//	uint16_t i;

	init_crc_table(crc_table);
	crc32 = 0xffffffff;
	for(offset=0; offset<iapHeadCrc1.data.appInfo.size; )
	{
		if(offset + W25Q128FV_SUBSECTOR_SIZE > iapHeadCrc1.data.appInfo.size)
		{
			len = iapHeadCrc1.data.appInfo.size - offset; 
		}
		else
		{
			len = W25Q128FV_SUBSECTOR_SIZE;
		}
		Mem_Read(APP_FILE_ADDR + offset, w25qx_buffer, len);
		crc32 = Crc32(crc_table, crc32, w25qx_buffer, len);
		offset += len;
		/*
		for (i=0;i<len;i++)
		{
			UART1_send_byte(w25qx_buffer[i]);
		}*/
	}

	crc32 = crc32^0xffffffffL;
	sprintf(str,"MY_CRC= %X   SYS_CRC= %X  \r\n",crc32,iapHeadCrc1.data.appInfo.crc32);
	Uart1_SendStr(str);
	
	if(crc32 != iapHeadCrc1.data.appInfo.crc32)
	{
		return false;
	}

	return true;
}


