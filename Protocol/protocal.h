#ifndef __PROTOCAL_H__
#define __PROTOCAL_H__
#include <stdbool.h>
#include <stdlib.h>
#include "stdint.h"
#include "stdio.h"
#include "string.h"
#include <stm32l1xx.h>
#include "iap.h"


#pragma pack(1)

typedef struct package {
    uint8_t type;   		//消息类型 4字节
    uint8_t ID[10];      	//id号，16字节
    uint32_t serial;		//序列号 4字节
} package;//only uint8_t ID[10] and uint8_t serial


typedef struct
{
    float temp1;//4
    float humi1;//4
    float temp2;//4
    float humi2;//4
    float temp3;//4
    float humi3;//4
    float press;//4
    float moisture;//4
    uint8_t batPercent;//1   33
    uint32_t ts;// 37
    uint8_t dig;//38
    uint8_t devicestatus;//40
    uint8_t runstatus;//41
    uint8_t protect;//42
} SensorSInfo;

typedef struct
{
    uint8_t ver[3];
    uint32_t size;
    uint32_t crc;
    uint32_t utc;
    uint32_t reqOffset;
    uint16_t reqLen;
    uint32_t ackOffset;
} UpdateFileInfo;


typedef struct sys_time {
//__IO uint32_t send_timeintval;//发送时间间隔
//__IO uint32_t collection_timeintval;//采集时间间隔
    __IO uint32_t time_stamp;//时间戳
} sys_time;

/*
enum sys_state
{
	DEV_STA_NOT_CALIBED=0,
	DEV_STA_CALIBEING=1,
	DEV_STA_CALIBED=2,
	DEV_STA_TRANS=3,
	DEV_STA_FACTORY_TEST=4,
	DEV_STA_ZERO_CALIBED=5
};
*/

enum sys_state
{
    DEV_STA_CALIBED=1,
    DEV_STA_FACTORY_TEST=2,
    DEV_STA_NOT_CALIBED=3,
    DEV_STA_TRANS=4,
    DEV_STA_CALIBEING=5,
    DEV_STA_ZERO_CALIBED=6
};
//1正常运行2 测试 3 待标定 4运输

uint8_t formatRealData(uint8_t* ip,uint8_t* port,float temp1,float humidity1,float temp2,float humidity2,float temp3,float humidity3,float pressure,uint8_t energy,uint32_t collect_time);
void ProcotalInit(void);
int SendPacket(uint8_t IPType,SensorSInfo par,uint32_t mess_type,uint8_t* ip,uint8_t* port);
void RecvServerPackage(uint16_t len,uint8_t* buffer,uint8_t IPType);
sys_time getSystime(void);
//void RTC_SetSystime(void);
void setSystime(sys_time newtime);
void ReSetSystime(void);
void SetSystime(uint32_t timestamp);
bool IsSensorDataSendSuccess(void);
void ResetSensorDataSendState(void);
void SetSensorDataSendState(void);
enum sys_state getState(void);
void SystimeAdd20(void);
void setID(uint8_t* id);
void LoraRcvHandler(uint8_t IPType);

#define MSG_TYPE_GET_TIME		1					//获取时间响应
#define MSG_TYPE_SENSOR_DATA	2
#define MSG_TYPE_SAMPLE_TIME	3					//请求升级信息
#define MSG_TYPE_CALIB_TIME		4					//标定
#define MSG_TYPE_CALIB_NONGDU	5
#define MSG_TYPE_CALIB_FINISH	6
#define MSG_TYPE_SENSOR_DATA_EX 7				//扩展上报
#define MSG_TYPE_UPDATE_DATA	8					//请求升级数据
//1正常运行2 测试 3 待标定 4运输

//#define UPDATE_DOWN_REQ_LEN			128
#define UPDATE_DOWN_REQ_LEN			64

#define LoraPort 0
#define BlueToothPort 1

#define Recv_SENSOR_DATA_EX_len 40
#define Recv_SAMPLE_TIME_len 27
#define Recv_CALIB_TIME_len 37
#define Recv_UPDATE_DATA_len 31

#include "STM32L151_Flash.h"
#include "delay.h"
#include "main.h"
#include "memMid.h"
#include "iap.h"
#include "memMgr.h"
#include "ProUart1.h"
#include "RTC_Alarm.h"
#include "Common.h"
#include "Lora.h"


#endif
