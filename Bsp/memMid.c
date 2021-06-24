#include "memMid.h"
#include "W25Qx.h"
#include "string.h"
#include <stdbool.h>
#include "usart.h"	  

#if 1

#define MEM_MID_WAIT_TIMEOUT_MS		(1000*5)
#define MEM_DEVICE_ENABLE			(1)

uint8_t w25qx_buffer[W25Q128FV_SUBSECTOR_SIZE] = {0};
//osSemaphoreId BinarySemMemHandle;


//static void BinarySemMemCreate(void)
//{
//  osSemaphoreDef(BinarySemMem);
//  BinarySemMemHandle = osSemaphoreCreate(osSemaphore(BinarySemMem), 1);
//}

uint8_t Mem_Init(void)
{
	uint8_t status = 0;
	#if MEM_DEVICE_ENABLE
	status = BSP_W25Qx_Init();
	#endif
	//BinarySemMemCreate();

	return status;
}

void Mem_Read_ID(uint8_t *ID)
{
	//osSemaphoreWait (BinarySemMemHandle, MEM_MID_WAIT_TIMEOUT_MS);
	#if MEM_DEVICE_ENABLE
	BSP_W25Qx_Read_ID( ID );
	#endif
	//osSemaphoreRelease(BinarySemMemHandle);
}

uint8_t Mem_Erase(uint32_t memAddress, uint16_t len)
{
	uint8_t status = 0;

	//osSemaphoreWait (BinarySemMemHandle, MEM_MID_WAIT_TIMEOUT_MS);
	#if MEM_DEVICE_ENABLE
	status = BSP_W25Qx_Erase_Block( memAddress );
	#endif
	//osSemaphoreRelease(BinarySemMemHandle);
	
	return status;
	
}

uint8_t Mem_Read(uint32_t memAddress, uint8_t *pData, uint16_t len)
{
	uint8_t status = 0;

	//osSemaphoreWait (BinarySemMemHandle, MEM_MID_WAIT_TIMEOUT_MS);
	#if MEM_DEVICE_ENABLE
	status = BSP_W25Qx_Read( pData, memAddress, len );
	#endif
	//osSemaphoreRelease(BinarySemMemHandle);

	return status;
}

uint8_t Mem_Write(uint32_t memAddress, uint8_t *pData, uint16_t len)
{
	uint8_t status = 0;
	
	//osSemaphoreWait (BinarySemMemHandle, MEM_MID_WAIT_TIMEOUT_MS);
	#if MEM_DEVICE_ENABLE
	status = BSP_W25Qx_Write( pData, memAddress, len );
	#endif
	//osSemaphoreRelease(BinarySemMemHandle);

	return status;
}

uint8_t Mem_Write_Ex(uint32_t memAddress, uint8_t *pData, uint16_t len)
{
	uint32_t subSectorsOffset = memAddress%W25Q128FV_SUBSECTOR_SIZE;
	uint32_t subSectorsStartAddr = memAddress -  subSectorsOffset;
	uint8_t status = 0;
	uint32_t i = 0;
	bool isNeedErase = false;

	//osSemaphoreWait (BinarySemMemHandle, MEM_MID_WAIT_TIMEOUT_MS);
	#if MEM_DEVICE_ENABLE
	status = BSP_W25Qx_Read(w25qx_buffer, subSectorsStartAddr, W25Q128FV_SUBSECTOR_SIZE);
	#endif
	if(W25Qx_OK != status)
	{
		//osSemaphoreRelease(BinarySemMemHandle);
		Uart1_SendStr("∂¡»°¥ÌŒÛ1£°\r\n");
		return status;
	}

	for(i=0; i<len; i++)
	{
		if( 0xFF != w25qx_buffer[subSectorsOffset+i] )
		{
			isNeedErase = true;
			break;
		}
	}
	
	if( isNeedErase )
	{
		memcpy( &w25qx_buffer[subSectorsOffset], pData, len);
		#if MEM_DEVICE_ENABLE
		status =BSP_W25Qx_Erase_Block( subSectorsStartAddr );
		if (status!=W25Qx_OK) 
		{
			Uart1_SendStr("≤¡≥˝¥ÌŒÛ£°\r\n");
			return status;
		}
		status = BSP_W25Qx_Write(w25qx_buffer, subSectorsStartAddr, W25Q128FV_SUBSECTOR_SIZE);
		if (status!=W25Qx_OK) 
		{
			Uart1_SendStr("–¥»Î¥ÌŒÛ£°\r\n");
		}		
		#endif
	}
	else
	{
		#if MEM_DEVICE_ENABLE
		status = BSP_W25Qx_Write( pData, memAddress, len );
		if (status!=W25Qx_OK) 
		{
			Uart1_SendStr("–¥»Î¥ÌŒÛ£°\r\n");
		}		
		#endif
	}

	//osSemaphoreRelease(BinarySemMemHandle);
	return status;
}

#endif

