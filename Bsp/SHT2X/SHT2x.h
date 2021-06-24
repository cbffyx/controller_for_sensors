#ifndef __SHT2x_H__
#define __SHT2x_H__
#include "delay.h"
#include "main.h"
#ifdef __cplusplus
 extern "C" {
#endif

#define  I2C_ADR_W 0x80  //SHT2X IIC 读地址
#define  I2C_ADR_R 0x81  //SHT2X IIC 写地址

	 
//================================================================================

////IO操作函数	 



//IIC所有操作函数
void IIC1_Init(void);                //初始化IIC的IO口				 
void IIC2_Init(void);                //初始化IIC的IO口				 
void IICP_Init(void);                //初始化IIC的IO口				 
void IIC1_Start(void);				//发送IIC开始信号
void IIC2_Start(void);				//发送IIC开始信号
void IICP_Start(void);				//发送IIC开始信号
void IIC1_Stop(void);	  			//发送IIC停止信号
void IIC2_Stop(void);	  			//发送IIC停止信号
void IICP_Stop(void);	  			//发送IIC停止信号
void IIC1_Send_Byte(uint8_t txd);			//IIC发送一个字节
void IIC2_Send_Byte(uint8_t txd);			//IIC发送一个字节
void IICP_Send_Byte(uint8_t txd);			//IIC发送一个字节
uint8_t IIC1_Read_Byte(unsigned char ack);//IIC读取一个字节
uint8_t IIC2_Read_Byte(unsigned char ack);//IIC读取一个字节
uint8_t IICP_Read_Byte(unsigned char ack);//IIC读取一个字节
uint8_t IIC1_Wait_Ack(void); 				//IIC等待ACK信号
uint8_t IIC2_Wait_Ack(void); 				//IIC等待ACK信号
uint8_t IICP_Wait_Ack(void); 				//IIC等待ACK信号
void IIC1_Ack(void);					//IIC发送ACK信号
void IIC2_Ack(void);					//IIC发送ACK信号
void IICP_Ack(void);					//IIC发送ACK信号
void IIC1_NAck(void);				//IIC不发送ACK信号
void IIC2_NAck(void);				//IIC不发送ACK信号
void IICP_NAck(void);				//IIC不发送ACK信号

//void IIC_Write_One_Byte(uint8_t daddr,uint8_t addr,uint8_t data);
//uint8_t IIC_Read_One_Byte(uint8_t daddr,uint8_t addr);	 
	 
//================================================================================	 
typedef enum {
    TRIG_TEMP_MEASUREMENT_HM   = 0xE3, // 触发 温度 测量 ，保持主机   命令
    TRIG_HUMI_MEASUREMENT_HM   = 0xE5, // 触发 湿度 测量 ，保持主机   命令
    TRIG_TEMP_MEASUREMENT_POLL = 0xF3, // 触发 温度 测量 ，非保持主机 命令
    TRIG_HUMI_MEASUREMENT_POLL = 0xF5, // 触发 湿度 测量 ，非保持主机 命令
    USER_REG_W                 = 0xE6, // 写寄存器命令
    USER_REG_R                 = 0xE7, // 读寄存器命令
    SOFT_RESET                 = 0xFE  // 软件复位命令
} SHT2xCommand;

typedef enum {
    SHT2x_RES_12_14BIT         = 0x00, //RH=12bit, T=14bit 这是默认的值   我们不用修改
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
uint8_t    SHT2x_SoftReset1(void);  //SHT20软件复位
uint8_t    SHT2x_SoftReset2(void);  //SHT20软件复位
uint8_t    SHT2x_SoftReset3(void);  //SHT20软件复位
float SHT2x_GetTempPoll1(void);//获取SHT20 温度
float SHT2x_GetTempPoll2(void);//获取SHT20 温度
float SHT2x_GetTempPoll3(void);//获取SHT20 温度
float SHT2x_GetHumiPoll1(void);//获取SHT20 湿度
float SHT2x_GetHumiPoll2(void);//获取SHT20 湿度
float SHT2x_GetHumiPoll3(void);//获取SHT20 湿度

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
