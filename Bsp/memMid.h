#ifndef MEM_MID_H
#define MEM_MID_H

#include <stdint.h>


uint8_t Mem_Init(void);
void Mem_Read_ID(uint8_t *ID);
uint8_t Mem_Read(uint32_t memAddress, uint8_t *pData, uint16_t len);
uint8_t Mem_Write(uint32_t memAddress, uint8_t *pData, uint16_t len);
uint8_t Mem_Erase(uint32_t memAddress, uint16_t len);
uint8_t Mem_Write_Ex(uint32_t memAddress, uint8_t *pData, uint16_t len);


#endif

