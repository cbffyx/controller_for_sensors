/**
  * @brief  ��ʪ�ȴ����� program.
  * @description None
  * @param  teper
  */

#include "SHT2x.h"
#include "delay.h"
#include <stdio.h>
#include "usart.h"

#define HT_VDD GPIO_Pin_12//GPIO_Pin_2 //GPIO_Pin_12
#define HT_SDA GPIO_Pin_7//GPIO_Pin_1 //GPIO_Pin_7
#define HT_SCK GPIO_Pin_6//GPIO_Pin_0 //GPIO_Pin_6

extern uint16_t GPIO_Pin_SCL;
extern uint16_t GPIO_Pin_SDA;	 
extern char GPIO_SCL_NUM;
extern char GPIO_SDA_NUM;

//IO��������
static void SDA_IN(){
	GPIO_InitTypeDef    GPIO_InitStructure;	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin = HT_SDA;//
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

static void SDA_OUT(){
	GPIO_InitTypeDef    GPIO_InitStructure;	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin = HT_SDA;//B0 EN B1 SDA B2 SCL
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType =GPIO_OType_OD;//��©���
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}
//==========================IIC===============================
//��ʼ��IIC
void IIC_Init(void)
{
    GPIO_InitTypeDef    GPIO_InitStructure;	
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = HT_SDA;//PB6 SCL PB7 SDA
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType =GPIO_OType_OD;//��©
    GPIO_Init(GPIOB, &GPIO_InitStructure);
	
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = HT_SCK;//PB6 SCL PB7 SDA
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType =GPIO_OType_PP;//�������
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void SDA_HIGH(){
  GPIO_SetBits(GPIOB,HT_SDA);
}

void SDA_LOW(){
  GPIO_ResetBits(GPIOB,HT_SDA);
}

void SCL_HIGH(){
  GPIO_SetBits(GPIOB,HT_SCK);
}

void SCL_LOW(){
  GPIO_ResetBits(GPIOB,HT_SCK);
}

uint8_t read_sdaval(){
   return GPIO_ReadInputDataBit(GPIOB,HT_SDA);
}

//����IIC��ʼ�ź�
void IIC_Start(void)
{
	SDA_OUT();     //sda�����
	SDA_HIGH();	  	  
	SCL_HIGH();
	delay_us(4);
 	SDA_LOW();//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	SCL_LOW();//ǯסI2C���ߣ�׼�����ͻ�������� 
	delay_us(4);
}	  
//����IICֹͣ�ź�
void IIC_Stop(void)
{
	SDA_OUT();//sda�����
	SCL_LOW();
	SDA_LOW();//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	SCL_HIGH(); 
	SDA_HIGH();//����I2C���߽����ź�
	delay_us(4);							   	
}
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
uint8_t IIC_Wait_Ack(void)
{
	uint8_t ucErrTime=0;
	SDA_IN();      //SDA����Ϊ����  
	//SDA_HIGH();
	//delay_us(1);	   
	SCL_HIGH();
	delay_us(1);	 
	while(read_sdaval())
	{
		ucErrTime++;
	//	delay_us(1);
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	SCL_LOW();//ʱ�����0 	   
	return 0;  
} 
//����ACKӦ��
void IIC_Ack(void)
{
	SCL_LOW();
	SDA_OUT();
	SDA_LOW();
	delay_us(20);
	SCL_HIGH();
	delay_us(2);
	SCL_LOW();
}
//������ACKӦ��		    
void IIC_NAck(void)
{
	SCL_LOW();
	SDA_OUT();
	SDA_HIGH();
	delay_us(5);
	SCL_HIGH();
	delay_us(5);
	SCL_LOW();
}					 				     
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
void IIC_Send_Byte(uint8_t txd)
{                        
    uint8_t t;   
	  SDA_OUT(); 	    
    SCL_LOW();//����ʱ�ӿ�ʼ���ݴ���
    for(t=0;t<8;t++)
    {              
			  if((txd&0x80)>>7){
				  SDA_HIGH();
        }
				else{
				  SDA_LOW();
        }
        txd<<=1; 	  
			  delay_us(5);   //��TEA5767��������ʱ���Ǳ����
			  SCL_HIGH();
			  delay_us(5); 
			  SCL_LOW();	
			  delay_us(5);
    }	 
} 	    
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
uint8_t IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA����Ϊ����
    for(i=0;i<8;i++ )
	{
    SCL_LOW(); 
    delay_us(5);
		SCL_HIGH();
    receive<<=1;
    if(read_sdaval()>0)receive++;   
		delay_us(5); 
    }					 
    if (!ack)
        IIC_NAck();//����nACK
    else
        IIC_Ack(); //����ACK   
    return receive;
}




//========================SHT2x(HTU21D)=======================

SHT2x_data SHT20;
void DisableSHT2(){
 GPIO_ResetBits(GPIOB,HT_VDD);
}
void EnableSHT2(){
	//оƬʹ�ܳ�ʼ��
	GPIO_InitTypeDef    GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = HT_VDD;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//push-pull
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);	
	GPIO_SetBits(GPIOB,HT_VDD);
}

void SHT2x_Init(void)
{
	IIC_Init();
}

uint8_t SHT2x_SoftReset(void) //SHT20�����λ
{
	uint8_t err=0;
	IIC_Start();
	IIC_Send_Byte(0x80);
	err = IIC_Wait_Ack();
	IIC_Send_Byte(0xFE);
	err = IIC_Wait_Ack();
	IIC_Stop();
	return err;
}
float SHT2x_GetTempPoll()
{
    float TEMP;
    uint8_t ack, tmp1, tmp2;
    uint16_t ST;
    uint16_t i=0;
	
    IIC_Start();				//����IIC��ʼ�ź�
    IIC_Send_Byte(I2C_ADR_W);			//IIC����һ���ֽ� 
		ack = IIC_Wait_Ack();	
		IIC_Send_Byte(TRIG_TEMP_MEASUREMENT_POLL);
		ack = IIC_Wait_Ack();

    do {
        delay_ms(10);               
        IIC_Start();				//����IIC��ʼ�ź�
        IIC_Send_Byte(I2C_ADR_R);	
			  i++;
			  ack = IIC_Wait_Ack();
			  if(i==100)break;
    } while(ack!=0);
		
    tmp1 = IIC_Read_Byte(1);
    tmp2 = IIC_Read_Byte(1);
		IIC_Read_Byte(0);
    IIC_Stop();
    ST = (tmp1 << 8) | (tmp2 << 0);
    ST &= ~0x0003;		
    TEMP = ((float)ST * 0.00268127) - 46.85;
    return (TEMP);	  
}

float SHT2x_GetHumiPoll()
{
    float HUMI;
    uint8_t ack, tmp1, tmp2;
    uint16_t SRH;
    uint16_t i=0;
    IIC_Start();				//����IIC��ʼ�ź�
    IIC_Send_Byte(I2C_ADR_W);			//IIC����һ���ֽ� 
    ack = IIC_Wait_Ack();	
    IIC_Send_Byte(TRIG_HUMI_MEASUREMENT_POLL);
    ack = IIC_Wait_Ack(); 
    do {
        delay_ms(10);               
        IIC_Start();				//����IIC��ʼ�ź�
        IIC_Send_Byte(I2C_ADR_R);	
        i++;
        ack = IIC_Wait_Ack();
        if(i==100)break;
    } while(ack!=0);
    
    tmp1 = IIC_Read_Byte(1); 
    tmp2 = IIC_Read_Byte(1);
    IIC_Read_Byte(0);
    IIC_Stop();
	
    SRH = (tmp1 << 8) | (tmp2 << 0);
    SRH &= ~0x0003;
		
    HUMI = ((float)SRH * 0.00190735) - 6;

    return (HUMI);
}


/****************** pressure ms5637 *************************/
#define MS5637_VDD GPIO_Pin_8
#define MS5637_SDA GPIO_Pin_7
#define MS5637_SCK GPIO_Pin_6

#define  MS5637_I2C_ADR_W 0xEC 
#define  MS5637_I2C_ADR_R 0xED  


MS5637Info ms5637Info;

void DisableMS5637()
{
    GPIO_ResetBits(GPIOB, MS5637_VDD);
}
void EnableMS5637()
{
    GPIO_InitTypeDef    GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = MS5637_VDD;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//push-pull
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//
    GPIO_Init(GPIOB,&GPIO_InitStructure);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);	
    GPIO_SetBits(GPIOB,MS5637_VDD);
}

static void IIC_WriteByte(uint8_t cmd)
{
	 uint8_t ack;
    IIC_Start();			
    IIC_Send_Byte(MS5637_I2C_ADR_W);			
    ack = IIC_Wait_Ack();	
    IIC_Send_Byte(cmd);
    ack = IIC_Wait_Ack();
    IIC_Stop();
}

void IIC_ReadBytes(uint8_t reg, uint8_t len, uint8_t *rData)
{
	uint8_t i = 0;
	uint8_t ack;
	IIC_Start();			
	IIC_Send_Byte(MS5637_I2C_ADR_W);			
	ack = IIC_Wait_Ack();	
	IIC_Send_Byte(reg);
	ack = IIC_Wait_Ack();
	IIC_Stop();
    
    do {
        delay_ms(10);               
        IIC_Start();				//����IIC��ʼ�ź�
        IIC_Send_Byte(MS5637_I2C_ADR_R);	
        i++;
        ack = IIC_Wait_Ack();
        if(i==100)break;
    } while(ack!=0);
#if 1
    for(i=0; i<len-1; i++)
    {
        *rData++ = IIC_Read_Byte(1);
    }
    if(len > 0)
    {
        *rData = IIC_Read_Byte(0);
    }
    #else
     for(i=0; i<len-1; i++)
    {
        *rData++ = IIC_Read_Byte(1);
    }
#endif    
    IIC_Stop();
}

uint8_t MS5637_SoftReset(void) //MS56370�����λ
{
    uint8_t err=0;
    IIC_Start();
    IIC_Send_Byte(MS5637_I2C_ADR_W);
    err = IIC_Wait_Ack();
    IIC_Send_Byte(0x1E);
    err = IIC_Wait_Ack();
    IIC_Stop();
    return err;
}

void i2c_master_init(void)
{
	IIC_Init();
}

enum status_code i2c_master_write_packet_wait(struct i2c_master_packet *packet)
{
	 uint8_t ack;
    IIC_Start();			
    IIC_Send_Byte(((packet->address)<<1)&0xFE);			
    ack = IIC_Wait_Ack();	
	if(ack != 0)
	{
		return STATUS_ERR_OVERFLOW;
	}
    IIC_Send_Byte(packet->data[0]);
    ack = IIC_Wait_Ack();
    IIC_Stop();	
    delay_ms(10);
	if(ack != 0)
	{
		return STATUS_ERR_OVERFLOW;
	}

	return STATUS_OK;
}

enum status_code i2c_master_read_packet_wait(struct i2c_master_packet *packet)
{
	uint8_t i = 0;
	uint8_t ack;
	
	do {
		delay_ms(10);				
		IIC_Start();				//����IIC��ʼ�ź�
		IIC_Send_Byte(((packet->address)<<1)|0x01);	
		i++;
		ack = IIC_Wait_Ack();
		if(i==100)break;
	} while(ack!=0);
	#if 1
	for(i=0; i<packet->data_length-1; i++)
	{
		packet->data[i] = IIC_Read_Byte(1);
	}
	if(packet->data_length > 0)
	{
		packet->data[packet->data_length-1] = IIC_Read_Byte(0);
	}
	#else
	 for(i=0; i<packet->data_length-1; i++)
	{
		packet->data[i] = IIC_Read_Byte(1);
	}
	#endif    
	IIC_Stop();
    delay_ms(10);
	if(1==ack)
	{
		return STATUS_ERR_OVERFLOW;
	}
	return STATUS_OK;
}

