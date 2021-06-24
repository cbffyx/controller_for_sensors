#ifndef __M5310_H__
#define __M5310_H__
#include "usart.h"
#include <stm32l1xx.h>
void PrintUsart3Buffer(void);//清空缓存	
void M5310_Init(void);
//void M5310_PDPACT(void);
void M5310_ConUDP(void);
void M5310UdpRcvHandler(uint16_t DelyTime,uint8_t mess_type,uint8_t IPType);
void M5310_Senddata(uint8_t len,uint8_t *data);
void M5310_backok(void);
void M5310_DisconUDP(void);
uint8_t Reconnect(void);
void AtCmd(void);
void ClosePSMeDRX(void);
void Connect(void);
void IncNetOffLineCnt(void);
void ResetNetOffLineCnt(void);
typedef struct
{
	uint8_t CSQ;    
	uint8_t Socketnum;   
	uint8_t reclen;   //获取到数据的长度
	uint8_t res;      
	uint16_t recdatalen;
} M5310;
void setID(uint8_t* id);
uint8_t getCSQ(void);
uint8_t *GetStringMiddle(uint8_t *str, uint16_t strlen, 
						char str1, uint16_t Nostr1, 
						char str2, uint16_t Nostr2, 
						uint16_t *substrlen);

#endif 

