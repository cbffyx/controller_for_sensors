#ifndef __SHT2x_H__
#define __SHT2x_H__
#include "delay.h"
#include "main.h"
#ifdef __cplusplus
 extern "C" {
#endif

#define  I2C_ADR_W 0x80  //SHT2X IIC ����ַ
#define  I2C_ADR_R 0x81  //SHT2X IIC д��ַ

	 
//================================================================================

////IO��������	 



//IIC���в�������
void IIC1_Init(void);                //��ʼ��IIC��IO��				 
void IIC2_Init(void);                //��ʼ��IIC��IO��				 
void IICP_Init(void);                //��ʼ��IIC��IO��				 
void IIC1_Start(void);				//����IIC��ʼ�ź�
void IIC2_Start(void);				//����IIC��ʼ�ź�
void IICP_Start(void);				//����IIC��ʼ�ź�
void IIC1_Stop(void);	  			//����IICֹͣ�ź�
void IIC2_Stop(void);	  			//����IICֹͣ�ź�
void IICP_Stop(void);	  			//����IICֹͣ�ź�
void IIC1_Send_Byte(uint8_t txd);			//IIC����һ���ֽ�
void IIC2_Send_Byte(uint8_t txd);			//IIC����һ���ֽ�
void IICP_Send_Byte(uint8_t txd);			//IIC����һ���ֽ�
uint8_t IIC1_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
uint8_t IIC2_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
uint8_t IICP_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
uint8_t IIC1_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
uint8_t IIC2_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
uint8_t IICP_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void IIC1_Ack(void);					//IIC����ACK�ź�
void IIC2_Ack(void);					//IIC����ACK�ź�
void IICP_Ack(void);					//IIC����ACK�ź�
void IIC1_NAck(void);				//IIC������ACK�ź�
void IIC2_NAck(void);				//IIC������ACK�ź�
void IICP_NAck(void);				//IIC������ACK�ź�

//void IIC_Write_One_Byte(uint8_t daddr,uint8_t addr,uint8_t data);
//uint8_t IIC_Read_One_Byte(uint8_t daddr,uint8_t addr);	 
	 
//================================================================================	 
typedef enum {
    TRIG_TEMP_MEASUREMENT_HM   = 0xE3, // ���� �¶� ���� ����������   ����
    TRIG_HUMI_MEASUREMENT_HM   = 0xE5, // ���� ʪ�� ���� ����������   ����
    TRIG_TEMP_MEASUREMENT_POLL = 0xF3, // ���� �¶� ���� ���Ǳ������� ����
    TRIG_HUMI_MEASUREMENT_POLL = 0xF5, // ���� ʪ�� ���� ���Ǳ������� ����
    USER_REG_W                 = 0xE6, // д�Ĵ�������
    USER_REG_R                 = 0xE7, // ���Ĵ�������
    SOFT_RESET                 = 0xFE  // �����λ����
} SHT2xCommand;

typedef enum {
    SHT2x_RES_12_14BIT         = 0x00, //RH=12bit, T=14bit ����Ĭ�ϵ�ֵ   ���ǲ����޸�
    SHT2x_RES_8_12BIT          = 0x01, //RH= 8bit, T=12bit
    SHT2x_RES_10_13BIT         = 0x80, //RH=10bit, T=13bit
    SHT2x_RES_11_11BIT         = 0x81, //RH=11bit, T=11bit
    SHT2x_RES_MASK             = 0x81  //Mask for res. bits (7,0) in user reg.
} SHT2xResolution;

typedef enum {
    SHT2x_HEATER_ON            = 0x04, //heater on
    SHT2x_HEATER_OFF           = 0x00, //heater off
    SHT2x_HEATER_MASK          = 0x04  //Mask for Heater bit(2) in user reg.
} SHT2xHeater;

typedef struct{
    float TEMP_POLL;
    float HUMI_POLL;    
} SHT2x_data;

enum status_code
{
	STATUS_OK,
	STATUS_ERR_OVERFLOW
};

struct i2c_master_packet {
	uint8_t address;
	uint8_t data_length;
	uint8_t *data;		 
};

extern SHT2x_data SHT20;

typedef struct
{
    float press;
    float cTemp;
    float fTemp;
}MS5637Info;

void SHT2x_Init(void); 
uint8_t    SHT2x_SoftReset1(void);  //SHT20�����λ
uint8_t    SHT2x_SoftReset2(void);  //SHT20�����λ
uint8_t    SHT2x_SoftReset3(void);  //SHT20�����λ
float SHT2x_GetTempPoll1(void);//��ȡSHT20 �¶�
float SHT2x_GetTempPoll2(void);//��ȡSHT20 �¶�
float SHT2x_GetTempPoll3(void);//��ȡSHT20 �¶�
float SHT2x_GetHumiPoll1(void);//��ȡSHT20 ʪ��
float SHT2x_GetHumiPoll2(void);//��ȡSHT20 ʪ��
float SHT2x_GetHumiPoll3(void);//��ȡSHT20 ʪ��

void DisableSHT2(void);
void EnableSHT2(void);

void DisableMS5637(void);
void EnableMS5637(void);

#ifndef HardBluetooth
void EnableSHT3(void);
void DisableSHT3(void);
#endif

void i2c_master_init(void);
uint8_t MS5637_SoftReset(void);
uint32_t MS5637_GetPressure(void);
enum status_code i2c_master_write_packet_wait(struct i2c_master_packet *packet);
enum status_code i2c_master_read_packet_wait(struct i2c_master_packet *packet);

void GetHumiTemp1(float *humi, float *temp);
void GetHumiTemp2(float *humi, float *temp);
void GetHumiTemp3(float *humi, float *temp);



#ifdef __cplusplus
}
#endif

#endif
