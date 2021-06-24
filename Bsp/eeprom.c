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
* ==EEPROM��ʼ������==
* FLASH_DeInit(); --> ��λEEPROM�ļĴ���ֵ
* FLASH_Unlock(FLASH_MEMTYPE_DATA); --> ���� �� Data EEPROM memory ���в���
* FLASH_SetProgrammingTime(FLASH_PROGRAMTIME_STANDARD); --> ��׼���ʱ��
*******************************************************************************/
void Init_EEPROM(void)
{
	FLASH_DeInit();
	FLASH_Unlock(FLASH_MEMTYPE_DATA);
	FLASH_SetProgrammingTime(FLASH_PROGRAMTIME_STANDARD);
}
 
/****************************************************************************************
*  Function: Derive_EPWrite                                                                        
*  Object: дEEPROM
*  ���룺 ͷ��ַ(0~2048)  ����ָ��  ���ݳ�
*  ����� ��                                         
*  ��ע:  2K EEPROM  ���ܳ���
****************************************************************************************/
void EEPROM_Write(uint16_t Adr, uint8_t *pData, uint16_t Len)
{
	uint8_t *p;
	//pָ��
    //ָ��EEPROM ��Ӧ�ĵ�Ԫ
	p = (uint8_t*)0x4000 + Adr;   
	//����
	FLASH_Unlock(FLASH_MEMTYPE_DATA);
	//д����
	for( ; Len > 0; Len--)
	{
			*p++ = *pData++;
			//�ȴ�д���
			while(!(FLASH_GetFlagStatus(FLASH_FLAG_EOP))); 
	} 
	//����
	FLASH_Lock(FLASH_MEMTYPE_DATA);
}
 
/****************************************************************************************
*  Function: Derive_EPRead                                                                         
*  Object: ��EEPROM
*  ���룺 ͷ��ַ(0~2048) ���ݴ��ָ�� ���ݳ�
*  ����� ��                                         
*  ��ע:  2K EEPROM
****************************************************************************************/
void EEPROM_Read(uint16_t Adr, uint8_t *pData, uint16_t Len)
{
	uint8_t *p;
	//pָ��ָ��EEPROM ��Ӧ�ĵ�Ԫ
	p = (uint8_t*)0x4000 + Adr; 
	//����
	FLASH_Unlock(FLASH_MEMTYPE_DATA);
	//������
	for( ; Len > 0; Len--)
	{
		*pData++ = *p++;
	}
	//����EEPROM
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
