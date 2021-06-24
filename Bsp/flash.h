#ifndef _FLASH_H_
#define _FLASH_H_


//-----------------------------------------------------------------------------
void  Flash_Config(void);
void  Flash_Write(uint32_t addr, uint8_t *p, uint8_t len);
void  Flash_Read( uint32_t addr, uint8_t *p, uint8_t len);


#endif