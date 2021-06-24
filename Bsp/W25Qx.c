/*********************************************************************************************************
*
* File                : ws_W25Qx.c
* Hardware Environment:
* Build Environment   : RealView MDK-ARM  Version: 4.20
* Version             : V1.0
* By                  :
*
*                                  (c) Copyright 2005-2011, WaveShare
*                                       http://www.waveshare.net
*                                          All Rights Reserved
*
*********************************************************************************************************/

#include "W25Qx.h"
//#include "spi.h"
#include "string.h"
#include "main.h"
#include "delay.h"
#include "usart.h"

#if 1

#define CS_Pin GPIO_Pin_4
#define CS_GPIO_Port GPIOA
#define SCLK_Pin GPIO_Pin_5
#define SCLK_GPIO_Port GPIOA
#define MISO_Pin GPIO_Pin_6
#define MISO_GPIO_Port GPIOA
#define MOSI_Pin GPIO_Pin_7
#define MOSI_GPIO_Port GPIOA


#define MOSI_H 		GPIO_WriteBit(MOSI_GPIO_Port, MOSI_Pin, Bit_SET)
#define MOSI_L 		GPIO_WriteBit(MOSI_GPIO_Port, MOSI_Pin, Bit_RESET)
#define SCLK_H 		GPIO_WriteBit(SCLK_GPIO_Port, SCLK_Pin, Bit_SET)
#define SCLK_L 		GPIO_WriteBit(SCLK_GPIO_Port, SCLK_Pin, Bit_RESET)
#define MISO 		GPIO_ReadInputDataBit(MISO_GPIO_Port, MISO_Pin)

#define W25Qx_Enable() 			GPIO_WriteBit(CS_GPIO_Port, CS_Pin, Bit_RESET);delay_ms(1)
#define W25Qx_Disable() 		GPIO_WriteBit(CS_GPIO_Port, CS_Pin, Bit_SET);delay_ms(1)

void W25QxInit(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

    /*Configure GPIO pins : PBPin PBPin PBPin */
    GPIO_InitStruct.GPIO_Pin = SCLK_Pin|MOSI_Pin;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP; //推挽复用输出
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_40MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Pin = CS_Pin;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP; //推挽复用输出
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_40MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    /*Configure GPIO pin : PtPin */
    GPIO_InitStruct.GPIO_Pin = MISO_Pin;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(MISO_GPIO_Port, &GPIO_InitStruct);

    /*Configure GPIO pin Output Level */
    GPIO_WriteBit(SCLK_GPIO_Port, SCLK_Pin, Bit_RESET);
    GPIO_WriteBit(MOSI_GPIO_Port, MOSI_Pin, Bit_RESET);

    /*Configure GPIO pin Output Level */
    GPIO_WriteBit(GPIOA, CS_Pin, Bit_SET);

    W25Qx_WAKEUP();
}


uint8_t SPI_SendByte(uint8_t data)
{
    uint8_t i = 0;
    uint8_t temp = 0;
    uint8_t tempdata;
    tempdata=data;

    for(i=8; i>0; i--)
    {
        if(tempdata&0x80)
        {
            MOSI_H;
        }
        else
        {
            MOSI_L;
        }
        tempdata<<=1;
        SCLK_H;
        __nop();
        __nop();
        __nop();
        temp<<=1;
        if(MISO)
        {
            temp++;
        }
        __nop();
        __nop();
        __nop();
        SCLK_L;
    }
    return temp;
}



uint8_t BSP_W25Qx_SPI_Transmit( uint8_t *pData, uint32_t Size )
{
    uint32_t i = 0;
    uint8_t status = W25Qx_OK;

    for(i=0; i<Size; i++)
    {
        SPI_SendByte( *(pData+i) );
        //pData++;
    }

    return status;
}


uint8_t BSP_W25Qx_SPI_Receive( uint8_t *pData, uint32_t Size )
{
    uint32_t i = 0;
    uint8_t status = W25Qx_OK;

    for(i=0; i<Size; i++)
    {
        *pData = SPI_SendByte(0xff);
        pData++;
    }

    return status;
}

/**
  * @brief  Initializes the W25Q128FV interface.
  * @retval None
  */
uint8_t BSP_W25Qx_Init(void)
{
    /* Reset W25Qxxx */
    BSP_W25Qx_Reset();

    return BSP_W25Qx_GetStatus();
}

/**
  * @brief  This function reset the W25Qx.
  * @retval None
  */
static void	BSP_W25Qx_Reset(void)
{
    uint8_t cmd[2] = {RESET_ENABLE_CMD,RESET_MEMORY_CMD};

    W25Qx_Enable();
    /* Send the reset command */
    BSP_W25Qx_SPI_Transmit(cmd, 2);
    W25Qx_Disable();

}

/**
  * @brief  Reads current status of the W25Q128FV.
  * @retval W25Q128FV memory status
  */
static uint8_t BSP_W25Qx_GetStatus(void)
{
    uint8_t cmd[] = {READ_STATUS_REG1_CMD};
    uint8_t status;

    W25Qx_Enable();
    /* Send the read status command */
    BSP_W25Qx_SPI_Transmit(cmd, 1);
    /* Reception of the data */
    BSP_W25Qx_SPI_Receive(&status, 1);
    W25Qx_Disable();

    /* Check the value of the register */
    if((status & W25Q128FV_FSR_BUSY) != 0)
    {
        return W25Qx_BUSY;
    }
    else
    {
        return W25Qx_OK;
    }
}

/**
  * @brief  This function send a Write Enable and wait it is effective.
  * @retval None
  */
static uint8_t BSP_W25Qx_WriteEnable(void)
{
    uint8_t cmd[] = {WRITE_ENABLE_CMD};
    uint32_t i = 0;

    /*Select the FLASH: Chip Select low */
    W25Qx_Enable();
    /* Send the read ID command */
    BSP_W25Qx_SPI_Transmit(cmd, 1);
    /*Deselect the FLASH: Chip Select high */
    W25Qx_Disable();

    /* Wait the end of Flash writing */
    i = 0;
    while(BSP_W25Qx_GetStatus() == W25Qx_BUSY)
    {
        /* Check for the Timeout */
        delay_ms(10);
        if(++i>=W25Qx_TIMEOUT_VALUE/10)
        {
            return W25Qx_TIMEOUT;
        }
    }

    return W25Qx_OK;
}

/**
  * @brief  Read Manufacture/Device ID.
	* @param  return value address
  * @retval None
  */
void BSP_W25Qx_Read_ID(uint8_t *ID)
{
    uint8_t cmd[4] = {READ_ID_CMD,0x00,0x00,0x00};//////////////////////

    W25Qx_Enable();
    /* Send the read ID command */
    BSP_W25Qx_SPI_Transmit(cmd, 4);	//4
    /* Reception of the data */
    BSP_W25Qx_SPI_Receive(ID, 2); //2
    W25Qx_Disable();
}

/**
  * @brief  Reads an amount of data from the QSPI memory.
  * @param  pData: Pointer to data to be read
  * @param  ReadAddr: Read start address
  * @param  Size: Size of data to read
  * @retval QSPI memory status
  */
uint8_t BSP_W25Qx_Read(uint8_t* pData, uint32_t ReadAddr, uint32_t Size)
{
    uint8_t cmd[4];

    /* Configure the command */
    cmd[0] = READ_CMD;
    cmd[1] = (uint8_t)(ReadAddr >> 16);
    cmd[2] = (uint8_t)(ReadAddr >> 8);
    cmd[3] = (uint8_t)(ReadAddr);

    W25Qx_Enable();
    /* Send the read ID command */
    BSP_W25Qx_SPI_Transmit(cmd, 4);
    /* Reception of the data */
    if (BSP_W25Qx_SPI_Receive(pData,Size) != W25Qx_OK)
    {
        return W25Qx_ERROR;
    }
    W25Qx_Disable();
    return W25Qx_OK;
}

/**
  * @brief  Writes an amount of data to the QSPI memory.
  * @param  pData: Pointer to data to be written
  * @param  WriteAddr: Write start address
  * @param  Size: Size of data to write,No more than 256byte.
  * @retval QSPI memory status
  */
uint8_t BSP_W25Qx_Write(uint8_t* ppData, uint32_t WriteAddr, uint32_t Size)
{
    uint8_t cmd[4];
    uint32_t end_addr, current_size, current_addr;
    //uint32_t tickstart = HAL_GetTick();
    uint32_t i = 0;

    /* Calculation of the size between the write address and the end of the page */
    current_addr = 0;

    while (current_addr <= WriteAddr)
    {
        current_addr += W25Q128FV_PAGE_SIZE;
    }
    current_size = current_addr - WriteAddr;

    /* Check if the size of the data is less than the remaining place in the page */
    if (current_size > Size)
    {
        current_size = Size;
    }

    /* Initialize the adress variables */
    current_addr = WriteAddr;
    end_addr = WriteAddr + Size;

    /* Perform the write page by page */
    do
    {
        /* Configure the command */
        cmd[0] = PAGE_PROG_CMD;
        cmd[1] = (uint8_t)(current_addr >> 16);
        cmd[2] = (uint8_t)(current_addr >> 8);
        cmd[3] = (uint8_t)(current_addr);

        /* Enable write operations */
        BSP_W25Qx_WriteEnable();

        W25Qx_Enable();
        /* Send the command */
        if (BSP_W25Qx_SPI_Transmit(cmd, 4) != W25Qx_OK)
        {
            return W25Qx_ERROR;
        }

        /* Transmission of the data */
        if (BSP_W25Qx_SPI_Transmit(ppData,current_size) != W25Qx_OK)
        {
            return W25Qx_ERROR;
        }
        W25Qx_Disable();
        /* Wait the end of Flash writing */
        i = 0;
        while(BSP_W25Qx_GetStatus() == W25Qx_BUSY)
        {
            /* Check for the Timeout */
            delay_ms(10);
            if(++i>=W25Qx_TIMEOUT_VALUE/10)
            {
                return W25Qx_TIMEOUT;
            }
        }

        /* Update the address and size variables for next page programming */
        current_addr += current_size;
        ppData += current_size;
        current_size = ((current_addr + W25Q128FV_PAGE_SIZE) > end_addr) ? (end_addr - current_addr) : W25Q128FV_PAGE_SIZE;
    } while (current_addr < end_addr);


    return W25Qx_OK;
}

/**
  * @brief  Erases the specified block of the QSPI memory.
  * @param  BlockAddress: Block address to erase
  * @retval QSPI memory status
  */
uint8_t BSP_W25Qx_Erase_Block(uint32_t Address)
{
    uint32_t i = 0;
    uint8_t cmd[4];

    cmd[0] = SECTOR_ERASE_CMD;
    cmd[1] = (uint8_t)(Address >> 16);
    cmd[2] = (uint8_t)(Address >> 8);
    cmd[3] = (uint8_t)(Address);

    /* Enable write operations */
    BSP_W25Qx_WriteEnable();

    /*Select the FLASH: Chip Select low */
    W25Qx_Enable();
    /* Send the read ID command */
    BSP_W25Qx_SPI_Transmit(cmd, 4);
    /*Deselect the FLASH: Chip Select high */
    W25Qx_Disable();

    /* Wait the end of Flash writing */
    i = 0;
    while(BSP_W25Qx_GetStatus() == W25Qx_BUSY)
    {
        /* Check for the Timeout */
        delay_ms(10);
        if(++i>=W25Q128FV_SECTOR_ERASE_MAX_TIME/10)
        {
            return W25Qx_TIMEOUT;
        }
    }
    return W25Qx_OK;
}

/**
  * @brief  Erases the entire QSPI memory.This function will take a very long time.
  * @retval QSPI memory status
  */
uint8_t BSP_W25Qx_Erase_Chip(void)
{
    uint32_t i = 0;
    uint8_t cmd[4];
    cmd[0] = SECTOR_ERASE_CMD;

    /* Enable write operations */
    BSP_W25Qx_WriteEnable();

    /*Select the FLASH: Chip Select low */
    W25Qx_Enable();
    /* Send the read ID command */
    BSP_W25Qx_SPI_Transmit(cmd, 1);
    /*Deselect the FLASH: Chip Select high */
    W25Qx_Disable();

    /* Wait the end of Flash writing */
    i = 0;
    while(BSP_W25Qx_GetStatus() != W25Qx_BUSY)
    {
        /* Check for the Timeout */
        delay_ms(10);
        if(++i>=W25Q128FV_BULK_ERASE_MAX_TIME/10)
        {
            return W25Qx_TIMEOUT;
        }
    }
    return W25Qx_OK;
}

void W25Qx_PowerDown(void)
{
    uint8_t cmd[] = {W25X_PowerDown};
    W25Qx_Enable();
    /* Send the read status command */
    BSP_W25Qx_SPI_Transmit(cmd, 1);
    W25Qx_Disable();
    delay_ms(100);
}

void W25Qx_WAKEUP(void)
{
    uint8_t cmd[] = {W25X_ReleasePowerDown};
    W25Qx_Enable();
    /* Send the read status command */
    BSP_W25Qx_SPI_Transmit(cmd, 1);
    W25Qx_Disable();
    delay_ms(1);
}

#if 0
uint8_t wData[0x100];
uint8_t rData[0x100];

void W25Qx_test( void )
{

    uint8_t ID[4] = {0};
    uint32_t i;
    uint8_t testStatus = 0;
    uint32_t j = 0;

    /*##-1- Read the device ID  ########################*/
    BSP_W25Qx_Init();
    delay_ms(1000);
    BSP_W25Qx_Read_ID(ID);

    /*##-2- Erase Block ##################################*/
    if(BSP_W25Qx_Erase_Block(0) == W25Qx_OK)
    {
        Uart1_SendStr(" QSPI Erase Block ok\r\n");
    }
    else
    {
        Uart1_SendStr(" QSPI Erase Block fail\r\n");
    }

    /*##-3- Read the flash     ########################*/
    if(BSP_W25Qx_Read(rData,0x00,0x100)== W25Qx_OK)
    {
        Uart1_SendStr(" QSPI Read ok\r\n\r\n");
    }

    /*##-2- Written to the flash ########################*/
    /* fill buffer */
    for(i =0; i<0x100; i ++)
    {
        wData[i] = i;
        rData[i] = 0;
    }

    if(BSP_W25Qx_Write(wData,0x00,0x100)== W25Qx_OK)
    {
        Uart1_SendStr(" QSPI Write ok\r\n");
    }
    else
    {
        Uart1_SendStr(" QSPI Write fail\r\n");
    }

    /*##-3- Read the flash     ########################*/
    if(BSP_W25Qx_Read(rData,0x00,0x100)== W25Qx_OK)
    {
        Uart1_SendStr(" QSPI Read ok\r\n\r\n");
    }

    /*##-4- check date          ########################*/
    if(memcmp(wData,rData,0x100) == 0 )
    {
        Uart1_SendStr(" W25Q128FV QuadSPI Test OK\r\n");
        testStatus = 1;
    }
    else
    {
        Uart1_SendStr(" W25Q128FV QuadSPI Test False\r\n");
        testStatus = 0;
    }
    //return;

#if 0
    for( j=0; j<5; j++)
    {
        /*##-2- Erase Block ##################################*/
        if( W25Qx_OK != BSP_W25Qx_Erase_Block(0x00) )
        {
            Error_Handler();
        }

        if(BSP_W25Qx_Read(w25qx_buffer, 0x00, W25Q128FV_SUBSECTOR_SIZE)!= W25Qx_OK)
        {
            Error_Handler();
        }

        for(i=0; i<15; i++)
        {
            memset(wData, i, 0x100);
            Mem_fill_Subsectors_buffer(i*0x100+j, wData, 0x100);
        }

        BSP_W25Qx_Write( w25qx_buffer, 0x00, W25Q128FV_SUBSECTOR_SIZE);

        if(BSP_W25Qx_Read(w25qx_buffer, 0x00, W25Q128FV_SUBSECTOR_SIZE)!= W25Qx_OK)
        {
            Error_Handler();
        }
    }
#endif

}
#endif

#endif

