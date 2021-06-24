#ifndef __L151FLASH_H
#define __L151FLASH_H 	

#include "stm32l1xx.h"

#define DATA_EEPROM_START_ADDR     0x08080000
//#define DATA_EEPROM_END_ADDR       0x080803FF
#define DATA_EEPROM_END_ADDR       0x08080FFF
#define DATA_EEPROM_PAGE_SIZE      0x8

#define IP1_Addr 						0x00		//生产服务器IP
#define Port1_Addr 					0x10		//生产服务器端口
#define IP2_Addr 						0x20		//正式服务器IP
#define Port2_Addr 					0x30		//正式服务器端口
//#define MotoStatues_Addr 		0x40		//电机状态
#define DeviceStatues_Addr 	0x48		//设备状态 1正常运行2 测试 3 待标定 4运输
#define RunStatues_Addr 		0x50		//运行状态 1 熏蒸准备 2 熏蒸中 3散气中 4 熏蒸结束 5未使用
//#define Enable_PH5_Addr			0x58		//电机闭锁状态
#define Fac_SN_Addr					0x60		//生产批次号
#define Dfrequency_Addr			0x68		//上报周期 分钟

#define ParamentOffset      0x0f00   //参数偏移

typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;
_Bool Has_biaoding(void);
void flash_init(void);
void myflashtest(void);
void writebdtest(void);

TestStatus InitList(void);
float getPH3Value(uint32_t Ph3Value);
TestStatus ListInsert(uint32_t Voltage,uint32_t Concentration);
void ResetZeroVoltage(uint32_t Voltage);

char getchar_cur(uint32_t cur);
uint8_t getu8_cur(uint32_t cur);

uint32_t GetZeroVoltage(void);
void SetZeroVoltage(uint32_t data);

void Set_IPPort1(char* ip,char* port);
void Get_IPPort1(char* ip,char* port);
void Set_IPPort2(char* ip,char* port);
void Get_IPPort2(char* ip,char* port);
void Set_DeviceStatue(uint8_t stats);
uint8_t Get_DeviceStatue(void);
void Set_RunStatue(uint8_t stats);
uint8_t Get_RunStatue(void);
void Set_EnablePH5(uint8_t stats);
uint8_t Get_EnablePH5(void);
void Set_Fac_SN(char* SN);
void Get_Fac_SN(char* SN);
void Set_CT(float CTValue);
float Get_CT(void);
void Set_CH(float CHValue);
float Get_CH(void);
void Set_CPH32k(float CPH32kValue);
float Get_CPH32k(void);
void Set_CPH35(float CPH35Value);
float Get_CPH35(void);
void Set_Dfrequency(uint16_t dat);
void Get_Dfrequency(uint16_t *dat);

#endif

