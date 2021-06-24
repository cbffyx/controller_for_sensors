
#ifndef GYRO_MEM_H
#define GYRO_MEM_H


#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "protocal.h"
#include "memMid.h"
#include "memMgr.h"
#include "w25qx.h"

#define   SENSOR_DATA_HEAD_FLAG    0x12 

#pragma pack(1)
typedef struct 
{
    uint8_t  sof;					//1		//起始符
    uint32_t seqNum;			//5		//顺序号
    uint32_t readAmount;	//9		//读数量
    uint32_t writeAmount; //13	//写数量
    uint32_t writePtr; 		//17	//写指针
    uint32_t readPtr; 		//21	//读指针
    uint8_t  rsv[10]; 		//31	//未定义保留 保证对齐
    uint8_t bcc;					//32	//异或校验
}SensorDataInfoMem; //must power 2

typedef struct 
{
    uint8_t  sof;
    SensorSInfo data;
    uint8_t  rsv[64-sizeof(SensorSInfo)-2];
    uint8_t bcc;
}SensorDataMem;//must power 2

#pragma pack()


void SensorMemInit(void);
bool ReadSensorData(SensorSInfo *data);
uint16_t GetSensorDataCnt(void);
void WriteSensorData(SensorSInfo *data);
void TagSensorDataSended(void);
void UpdateSensorInfoMem(void);
void ClearSensorMemSafe(void);




#ifdef __cplusplus
}
#endif


#endif


