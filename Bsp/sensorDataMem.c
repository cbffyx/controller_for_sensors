/*
**  by zeaho
*/
#include "sensorDataMem.h"

static SensorDataInfoMem sensorDataInfo;
static SensorDataInfoMem sensorDataInfoInit = {
    .sof = SENSOR_DATA_HEAD_FLAG,
    .seqNum = 0,
    .readAmount = 0,
    .writeAmount = 0,
    .writePtr = SENSOR_DATA_ADDR,
    .readPtr = SENSOR_DATA_ADDR,
    };
static uint32_t sensorInfoNextAddr = SENSOR_DATA_INFO_ADDR;

static uint8_t BccHash(const uint8_t *data_ptr, uint32_t size)
{
    uint8_t bcc = 0;
    for(int i = 0; i < size; ++i)
    {
        bcc = bcc ^ data_ptr[i];
    }

    return bcc;
}

/******************************************************************************
 **  UpdateSensorInfoMem  // HEAD + DATA + BCC
 **-----------------------------------------------------------------------------
 *******************************************************************************/
void UpdateSensorInfoMem(void)
{
    sensorDataInfo.sof = SENSOR_DATA_HEAD_FLAG;
    sensorDataInfo.seqNum++;
    memset(sensorDataInfo.rsv, 0xff, sizeof(sensorDataInfo.rsv));
    sensorDataInfo.bcc = BccHash((const uint8_t *)&sensorDataInfo, sizeof(SensorDataInfoMem)-1);
    if(sensorInfoNextAddr >= SENSOR_DATA_INFO_ADDR+SENSOR_DATA_INFO_SIZE)
    {
        sensorInfoNextAddr = SENSOR_DATA_INFO_ADDR;
    }
    Mem_Write_Ex(sensorInfoNextAddr, (uint8_t *)&sensorDataInfo, sizeof(SensorDataInfoMem));
    sensorInfoNextAddr += sizeof(SensorDataInfoMem);
}

/******************************************************************************
 **  WriteToSensorMem //边界最后一个不处理   pDataLen仅参考
 *******************************************************************************/
static void WriteToSensorMem(uint8_t *pData, uint16_t pDataLen)
{
	if(sensorDataInfo.writePtr >= (SENSOR_DATA_ADDR+ SENSOR_DATA_SIZE))
	{
		sensorDataInfo.writePtr = sensorDataInfoInit.writePtr;
	}
	Mem_Write_Ex(sensorDataInfo.writePtr, pData, sizeof(SensorDataMem));
	sensorDataInfo.writePtr += sizeof(SensorDataMem);
	sensorDataInfo.writeAmount++;
    UpdateSensorInfoMem();
}

/******************************************************************************
 ** ReadSensorInfoMemSafe // HEAD + DATA + BCC
 **-----------------------------------------------------------------------------
 *******************************************************************************/
static void ReadSensorInfoMemSafe(void)
{
    uint8_t bcc = 0;
    uint32_t i = 0;
    uint32_t seqMax = 0;
    SensorDataInfoMem sensorInfo;
    bool isSensorBootExist = false;
    uint32_t offset = 0;

    for(i=0; i<SENSOR_DATA_INFO_SIZE/sizeof(SensorDataInfoMem); i++)
    {
        offset = SENSOR_DATA_INFO_ADDR+i*sizeof(SensorDataInfoMem);
        Mem_Read(offset, (uint8_t *)&sensorInfo, sizeof(SensorDataInfoMem));
        bcc = BccHash((const uint8_t *)&sensorInfo, sizeof(SensorDataInfoMem)-1);
        if( (bcc==sensorInfo.bcc) && (SENSOR_DATA_HEAD_FLAG==sensorInfo.sof) )
        {
            if(sensorInfo.seqNum >= seqMax)
            {
                memcpy(&sensorDataInfo, &sensorInfo, sizeof(SensorDataInfoMem)); 
                seqMax = sensorInfo.seqNum;
                isSensorBootExist = true;
                sensorInfoNextAddr = offset+sizeof(SensorDataInfoMem);
            }
        }
    }

    if(!isSensorBootExist || (sensorDataInfo.writeAmount<sensorDataInfo.readAmount))
    {
        memcpy(&sensorDataInfo, &sensorDataInfoInit, sizeof(SensorDataInfoMem));
        sensorInfoNextAddr = SENSOR_DATA_INFO_ADDR;
    }
}

/******************************************************************************
 ** ReadSensorData //最后一条不存储不读取 是否外部函数处理还是内部处理
 *******************************************************************************/
bool ReadSensorData(SensorSInfo *data)
{
    uint8_t bcc = 0;
    SensorDataMem sensorMem;
	bool isValid = false;

    if(sensorDataInfo.readPtr >= (SENSOR_DATA_ADDR+ SENSOR_DATA_SIZE))
    {
        sensorDataInfo.readPtr = sensorDataInfoInit.readPtr;
    }

    Mem_Read(sensorDataInfo.readPtr, (uint8_t *)&sensorMem, sizeof(SensorDataMem));
    bcc = BccHash((const uint8_t *)&sensorMem, sizeof(SensorDataMem)-1); 
	if( (SENSOR_DATA_HEAD_FLAG==sensorMem.sof) && (bcc==sensorMem.bcc) ) 
    {    
		*data = sensorMem.data;
		isValid = true;
    }

    //sensorDataInfo.readPtr += sizeof(SensorDataMem);
 	//sensorDataInfo.readAmount++;

    return isValid;

}

/******************************************************************************
 ** ClearSensorMemSafe
 **-----------------------------------------------------------------------------
 *******************************************************************************/
void ClearSensorMemSafe(void)
{
    Mem_Erase( SENSOR_DATA_INFO_ADDR, W25Q128FV_SUBSECTOR_SIZE);
    sensorInfoNextAddr = SENSOR_DATA_INFO_ADDR;
    memcpy(&sensorDataInfo, &sensorDataInfoInit, sizeof(SensorDataInfoMem));
    UpdateSensorInfoMem();
}

/******************************************************************************
 ** IsSensorMemNub
 **-----------------------------------------------------------------------------
 *******************************************************************************/
uint16_t GetSensorDataCnt(void)
{
    uint32_t amount = 0;

    if(sensorDataInfo.writeAmount >= sensorDataInfo.readAmount)
    {
        amount = (sensorDataInfo.writeAmount - sensorDataInfo.readAmount);
    }
    return amount;
}

void WriteSensorData(SensorSInfo *data)
{
    uint8_t wrBuff[sizeof(SensorDataMem)] = {0};
    SensorDataMem *sensor = (SensorDataMem *)&wrBuff[0];
    
    sensor->sof = SENSOR_DATA_HEAD_FLAG;
    memcpy(&sensor->data, data, sizeof(SensorSInfo));
    memset(sensor->rsv, 0xff, sizeof(sensor->rsv));
    sensor->bcc = BccHash(wrBuff, sizeof(SensorDataMem)-1);
    WriteToSensorMem(wrBuff, sizeof(SensorDataMem));
}

/******************************************************************************
 ** SensorMemInit
 *******************************************************************************/
void SensorMemInit(void)
{
    ReadSensorInfoMemSafe();
}

void TagSensorDataSended(void)
{
	sensorDataInfo.readPtr += sizeof(SensorDataMem);
	sensorDataInfo.readAmount++;
	UpdateSensorInfoMem();
}



