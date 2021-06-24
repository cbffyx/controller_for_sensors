#ifndef __ProUart1_H
#define __ProUart1_H
#include <stm32l1xx.h>
#include "protocal.h"
#include "stdio.h"	
#include "stdint.h"
#include "string.h"
#include <stdbool.h>
#include "usart.h"	  
#include "memMid.h"
#include "W25Qx.h"

#define MaxBDCount 			20	 //最多标定点
#define WD_BD_Addr1   		0x00040000	//温度1标定数据地址
#define SD_BD_Addr1   		0x00041000	//湿度1标定数据地址
#define WD_BD_Addr2				0x00042000	//温度2标定数据地址
#define SD_BD_Addr2			 	0x00043000	//湿度2标定数据地址 
#define WD_BD_Addr3				0x00044000	//温度2标定数据地址
#define SD_BD_Addr3			 	0x00045000	//湿度2标定数据地址 
#define QY_BD_Addr   		0x00046000	//气压标定数据地址
#define SF_BD_Addr   		0x00047000	//水份标定数据地址
#define BAT_BD_Addr   		0x00048000	//水份标定数据地址
#define Loar_Info_Addr	0x00070000	//LoraInfo APPEUI APPKEY
#define APPEUI_Len 			16
#define APPKEY_Len 			32

typedef struct
{
	float Rdata;
	float Bdata;
}BDData;

typedef struct
{
	uint8_t BdFlag;
	uint8_t BdCount;
	BDData BdData[MaxBDCount];
}BD;

uint8_t AddBD(uint32_t Addr,float RealData,float NowData);//加温度标定点
float JS(uint32_t Addr,float data);
void ClearBD(uint32_t Addr);
uint8_t ReadBiaoDingData(uint32_t Addr,BD *Data);
void Read_APPEUI_APPKEY(char * APPEUI,char * APPKEY);
void Write_APPEUI_APPKEY(char * APPEUI,char * APPKEY);
bool DealUart(uint8_t *RecFuf,uint16_t RcvLen);
#endif

