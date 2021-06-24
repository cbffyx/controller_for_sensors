
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
    uint8_t  sof;					//1		//��ʼ��
    uint32_t seqNum;			//5		//˳���
    uint32_t readAmount;	//9		//������
    uint32_t writeAmount; //13	//д����
    uint32_t writePtr; 		//17	//дָ��
    uint32_t readPtr; 		//21	//��ָ��
    uint8_t  rsv[10]; 		//31	//δ���屣�� ��֤����
    uint8_t bcc;					//32	//���У��
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


