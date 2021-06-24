/**
  ******************************************************************************
  * @file    Bsp/eeprom.c 
  * @author  cbf
  * @version V0.0.0
  * @date    7-7-2019
  * @brief   function control by gpio
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"
#include "eeprom.h"
#include "stm8s_flash.h"
 
 
/*******************************************************************************
* ==EEPROM初始化函数==
* FLASH_DeInit(); --> 复位EEPROM的寄存器值
* FLASH_Unlock(FLASH_MEMTYPE_DATA); --> 解锁 对 Data EEPROM memory 进行操作
* FLASH_SetProgrammingTime(FLASH_PROGRAMTIME_STANDARD); --> 标准编程时间
*******************************************************************************/
void Init_EEPROM(void)
{
	FLASH_DeInit();
	FLASH_Unlock(FLASH_MEMTYPE_DATA);
	FLASH_SetProgrammingTime(FLASH_PROGRAMTIME_STANDARD);
}
 
/****************************************************************************************
*  Function: Derive_EPWrite                                                                        
*  Object: 写EEPROM
*  输入： 头地址(0~2048)  数据指针  数据长
*  输出： 无                                         
*  备注:  2K EEPROM  不能超过
****************************************************************************************/
void EEPROM_Write(uint16_t Adr, uint8_t *pData, uint16_t Len)
{
	uint8_t *p;
	//p指针
    //指向EEPROM 对应的单元
	p = (uint8_t*)0x4000 + Adr;   
	//解锁
	FLASH_Unlock(FLASH_MEMTYPE_DATA);
	//写数据
	for( ; Len > 0; Len--)
	{
			*p++ = *pData++;
			//等待写完成
			while(!(FLASH_GetFlagStatus(FLASH_FLAG_EOP))); 
	} 
	//加锁
	FLASH_Lock(FLASH_MEMTYPE_DATA);
}
 
/****************************************************************************************
*  Function: Derive_EPRead                                                                         
*  Object: 读EEPROM
*  输入： 头地址(0~2048) 数据存放指针 数据长
*  输出： 无                                         
*  备注:  2K EEPROM
****************************************************************************************/
void EEPROM_Read(uint16_t Adr, uint8_t *pData, uint16_t Len)
{
	uint8_t *p;
	//p指针指向EEPROM 对应的单元
	p = (uint8_t*)0x4000 + Adr; 
	//解锁
	FLASH_Unlock(FLASH_MEMTYPE_DATA);
	//读数据
	for( ; Len > 0; Len--)
	{
		*pData++ = *p++;
	}
	//加锁EEPROM
	FLASH_Lock(FLASH_MEMTYPE_DATA);
}


uint8_t get_stored_status(void)
{
    uint8_t sts_val;  
    EEPROM_Read(0x16, &sts_val, 1);
    return sts_val;
}

void set_stored_status(uint8_t sts_val)
{  
    EEPROM_Write(0x16, &sts_val, 1);
}




/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
