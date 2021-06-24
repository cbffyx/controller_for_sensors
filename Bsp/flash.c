#include "my_type.h"


Union_Define my_union={0};
//-----------------------------------------------------------------------------
void Flash_Write(uint32_t addr, uint8_t *p, uint8_t len)
{
    uint8_t buf[10]={0};
    uint8_t i=0;
    
    for(i=0;i<len;i++)
    {
        buf[i] = FLASH_ReadByte(addr+i);	
        if(buf[i] != p[i])
        {
            FLASH_EraseByte(addr+i); 
            FLASH_ProgramByte(addr+i, p[i]); 
        }
    }   
}

//-----------------------------------------------------------------------------
void Flash_Read(uint32_t addr, uint8_t *p, uint8_t len)
{
    uint8_t i=0;
    
    for(i=0;i<len;i++)
    {
        *p++ = FLASH_ReadByte(addr+i);	
    }   
}

//-----------------------------------------------------------------------------
void Flash_Config(void)
{  
    FLASH_SetProgrammingTime(FLASH_ProgramTime_TProg);
    FLASH_Unlock(FLASH_MemType_Data);
    while(FLASH_GetFlagStatus(FLASH_FLAG_DUL) == RESET);
    
    FLASH->CR2 |= FLASH_CR2_OPT;
    asm("nop");
    *((uchar *)0x480b) = 0x55;
    asm("nop");
    *((uchar *)0x480c) = 0xaa;                  //写入选项字节
    FLASH->CR2 &= (uint8_t)(~FLASH_CR2_OPT);
    
    Flash_Read( eeprom+0, (uint8_t *)my_union.buf, 2);            
    if((my_union.temp==0)||(my_union.temp==U16_MAX)) 
    {
        my_union.temp = 1;
        BSP.DZ = my_union.temp;                 //地址修改位
        Flash_Write( eeprom+0, (uint8_t *)my_union.buf, 2);      
    }
    else BSP.DZ = my_union.temp; 
    
    Flash_Read( eeprom+2, (uint8_t *)my_union.buf, 2);            
    if((my_union.temp==0)||(my_union.temp==U16_MAX)) 
    {
        my_union.temp = 60;
        BSP.Every_time = my_union.temp;         //发送周期修改位
        Flash_Write( eeprom+2, (uint8_t *)my_union.buf, 2);      
    }
    else BSP.Every_time = my_union.temp; 
    
    Flash_Read( eeprom+4, (uint8_t *)my_union.buf, 2);            
    if((my_union.temp==0)||(my_union.temp==U16_MAX)) 
    {
        my_union.temp = 480;
        BSP.Every_times = my_union.temp;         //必发周期修改位
        Flash_Write( eeprom+4, (uint8_t *)my_union.buf, 2);      
    }
    else BSP.Every_times = my_union.temp; 
}
