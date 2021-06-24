#ifndef __L151FLASH_H
#define __L151FLASH_H 	

#include "stm32l1xx.h"

#define DATA_EEPROM_START_ADDR     0x08080000
//#define DATA_EEPROM_END_ADDR       0x080803FF
#define DATA_EEPROM_END_ADDR       0x08080FFF
#define DATA_EEPROM_PAGE_SIZE      0x8

#define IP1_Addr 						0x00		//����������IP
#define Port1_Addr 					0x10		//�����������˿�
#define IP2_Addr 						0x20		//��ʽ������IP
#define Port2_Addr 					0x30		//��ʽ�������˿�
//#define MotoStatues_Addr 		0x40		//���״̬
#define DeviceStatues_Addr 	0x48		//�豸״̬ 1��������2 ���� 3 ���궨 4����
#define RunStatues_Addr 		0x50		//����״̬ 1 Ѭ��׼�� 2 Ѭ���� 3ɢ���� 4 Ѭ������ 5δʹ��
//#define Enable_PH5_Addr			0x58		//�������״̬
#define Fac_SN_Addr					0x60		//�������κ�
#define Dfrequency_Addr			0x68		//�ϱ����� ����

#define ParamentOffset      0x0f00   //����ƫ��

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

