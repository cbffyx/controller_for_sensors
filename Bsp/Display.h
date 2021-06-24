#ifndef __Display_H
#define __Display_H
#include "stm32l1xx.h"

#define MONOMSB_MODE 1
#define MONOLSB_MODE 2 
#define RED_MODE     3


#define MAX_LINE_BYTES 16// =128/8
#define MAX_COLUMN_BYTES  296

#define ALLSCREEN_GRAGHBYTES	4736

#define MONO 1                  //ºÚ°×
#define RED  2                  //ÈýÉ«£¨ºÚ°×ºì¡¢ºÚ°×»ÆÍ¨ÓÃ£©

///////////////////////////GPIO Settings//////////////////////////////////////////////////////
#define EPD_W21_RST_Pin GPIO_Pin_8
#define EPD_W21_RST_Port GPIOA
#define EPD_W21_MOSI_Pin GPIO_Pin_12
#define EPD_W21_MOSI_Port GPIOB
#define EPD_W21_CLK_Pin GPIO_Pin_14
#define EPD_W21_CLK_Port GPIOB
#define EPD_W21_CS_Pin GPIO_Pin_15
#define EPD_W21_CS_Port GPIOB
#define EPD_W21_DC_Pin GPIO_Pin_13
#define EPD_W21_DC_Port GPIOB
#define EPD_W21_BUSY_Pin GPIO_Pin_0
#define EPD_W21_BUSY_Port GPIOB


#define EPD_W21_MOSI_0	GPIO_ResetBits(EPD_W21_MOSI_Port, EPD_W21_MOSI_Pin)
#define EPD_W21_MOSI_1	GPIO_SetBits(EPD_W21_MOSI_Port, EPD_W21_MOSI_Pin)

#define EPD_W21_CLK_0	GPIO_ResetBits(EPD_W21_CLK_Port, EPD_W21_CLK_Pin)
#define EPD_W21_CLK_1	GPIO_SetBits(EPD_W21_CLK_Port, EPD_W21_CLK_Pin)

#define EPD_W21_CS_0	GPIO_ResetBits(EPD_W21_CS_Port, EPD_W21_CS_Pin)
#define EPD_W21_CS_1	GPIO_SetBits(EPD_W21_CS_Port, EPD_W21_CS_Pin)

#define EPD_W21_DC_0	GPIO_ResetBits(EPD_W21_DC_Port, EPD_W21_DC_Pin)
#define EPD_W21_DC_1	GPIO_SetBits(EPD_W21_DC_Port, EPD_W21_DC_Pin)

#define EPD_W21_RST_0	GPIO_ResetBits(EPD_W21_RST_Port, EPD_W21_RST_Pin)
#define EPD_W21_RST_1	GPIO_SetBits(EPD_W21_RST_Port, EPD_W21_RST_Pin)

#define EPD_W21_BUSY_LEVEL 0
#define isEPD_W21_BUSY GPIO_ReadInputDataBit(EPD_W21_BUSY_Port, EPD_W21_BUSY_Pin) // for solomen solutions


extern void driver_delay_xms(unsigned long xms);

void EpaperIO_Init(void);
void Epaper_READBUSY(void);
void Epaper_Spi_WriteByte(unsigned char TxData);
void Epaper_Write_Command(unsigned char cmd);
void Epaper_Write_Data(unsigned char data);

void EPD_HW_Init(void); //Electronic paper initialization
void EPD_Update(void);
	
void EPD_WhiteScreen_Red(void);
void EPD_WhiteScreen_Black(void);
void EPD_WhiteScreen_White(void);
void EPD_DeepSleep(void);
//Display 

void EPD_ALL_image(const unsigned char *datas1,const unsigned char *datas2);
void EPD_Dis_Part(unsigned int x_start,unsigned int y_start,const unsigned char * datas,const unsigned char color_mode,unsigned int PART_COLUMN,unsigned int PART_LINE);
void EPD_Fill_Part(unsigned int x_start,unsigned int y_start,const unsigned char data,const unsigned char color_mode,unsigned int PART_COLUMN,unsigned int PART_LINE);
void EPD_Dis_Part_mult(unsigned int x_startA,unsigned int y_startA,const unsigned char * datasA1,const unsigned char * datasA2,
	                     unsigned int x_startB,unsigned int y_startB,const unsigned char * datasB1,const unsigned char * datasB2,
											 unsigned int PART_COLUMN,unsigned int PART_LINE);

void Display_Init(void);
void DisplayTempHumi(float Temp,float Humi);
void DisplayDebug(float Temp1,float Humi1,float Temp2,float Humi2,float Temp3,float Humi3,float pressure);
void DisplayGPIO_Init(void);
void QRGenerator(char *input);
#endif /* __Display_H */

