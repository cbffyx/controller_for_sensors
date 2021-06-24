/**
  ******************************************************************************
  * @file    eeprom.h
  * @author  cbf
  * @version V1.0.0
  * @date    7-7-2019
  * @brief   
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _EEPROM_CONTROL_H_
#define _EEPROM_CONTROL_H_

/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"


void Init_EEPROM(void);

uint8_t get_stored_status(void);

void set_stored_status(uint8_t sts_val);

#endif /* _AD_CONTROL_H_ */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/