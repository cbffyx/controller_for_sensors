/**
  * @brief  温湿度传感器 program.
  * @description None
  * @param  teper
  */

#include "SHT2x.h"
#include "delay.h"
#include <stdio.h>
#include "usart.h"
#include "main.h"

//温湿度电源
#define HT_VDD GPIO_Pin_12
#define HT_VDD_P GPIOA
//温湿度1 IIC
#define HT_SDA1 GPIO_Pin_7
#define HT_SDA1_P GPIOB
#define HT_SCK1 GPIO_Pin_6
#define HT_SCK1_P GPIOB
//温湿度2 IIC
#define HT_SDA2 GPIO_Pin_5
#define HT_SDA2_P GPIOB
#define HT_SCK2 GPIO_Pin_4
#define HT_SCK2_P GPIOB

//气压
#define P_VDD GPIO_Pin_8
#define P_VDD_P GPIOB

#define P_SDA GPIO_Pin_3
#define P_SDA_P GPIOB
#define P_SCK GPIO_Pin_2
#define P_SCK_P GPIOB

extern uint8_t TimeOutSign;
		
//IO方向设置
static void SDA1_IN(){
	GPIO_InitTypeDef    GPIO_InitStructure;	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin = HT_SDA1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(HT_SDA1_P, &GPIO_InitStructure);
}

static void SDA1_OUT(){
	GPIO_InitTypeDef    GPIO_InitStructure;	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin = HT_SDA1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType =GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(HT_SDA1_P, &GPIO_InitStructure);
}

static void SDA2_IN(){
	GPIO_InitTypeDef    GPIO_InitStructure;	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin = HT_SDA2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(HT_SDA2_P, &GPIO_InitStructure);
}

static void SDA2_OUT(){
	GPIO_InitTypeDef    GPIO_InitStructure;	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin = HT_SDA2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType =GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(HT_SDA2_P, &GPIO_InitStructure);
}

static void SDAP_IN(){
	GPIO_InitTypeDef    GPIO_InitStructure;	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin = P_SDA;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(P_SDA_P, &GPIO_InitStructure);
}

#ifdef HardBluetooth
  #define SDA3_IN SDAP_IN
#else
void SDA3_IN(void){
	GPIO_InitTypeDef    GPIO_InitStructure;	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}
#endif


static void SDAP_OUT(){
	GPIO_InitTypeDef    GPIO_InitStructure;	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin = P_SDA;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType =GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(P_SDA_P, &GPIO_InitStructure);
}

#ifdef HardBluetooth
  #define SDA3_OUT SDAP_OUT
#else
void SDA3_OUT(){
	GPIO_InitTypeDef    GPIO_InitStructure;	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType =GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}
#endif

//==========================IIC===============================
//初始化IIC
void IIC1_Init(void)
{
    GPIO_InitTypeDef    GPIO_InitStructure;	
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = HT_SDA1 | HT_SCK1 ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType =GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
	
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin = HT_VDD;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType =GPIO_OType_PP;//推挽输出
    GPIO_Init(HT_VDD_P, &GPIO_InitStructure);
}

void IIC2_Init(void)
{
    GPIO_InitTypeDef    GPIO_InitStructure;	
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = HT_SDA2 | HT_SCK2 ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType =GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
	
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin = HT_VDD;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType =GPIO_OType_PP;//推挽输出
    GPIO_Init(HT_VDD_P, &GPIO_InitStructure);
}

void IICP_Init(void)
{
    GPIO_InitTypeDef    GPIO_InitStructure;	
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin =  P_VDD |	P_SDA | P_SCK;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType =GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

#ifdef HardBluetooth
  #define IIC3_Init IICP_Init
#else
void IIC3_Init(void)
{
    GPIO_InitTypeDef    GPIO_InitStructure;	
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_2 |	GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType =GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
	
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType =GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}
#endif

void SDA1_HIGH(){
  GPIO_SetBits(HT_SDA1_P,HT_SDA1);
}

void SDA1_LOW(){
  GPIO_ResetBits(HT_SDA1_P,HT_SDA1);
}

void SCL1_HIGH(){
  GPIO_SetBits(HT_SCK1_P,HT_SCK1);
}

void SCL1_LOW(){
  GPIO_ResetBits(HT_SCK1_P,HT_SCK1);
}

void SDA2_HIGH(){
  GPIO_SetBits(HT_SDA2_P,HT_SDA2);
}

void SDA2_LOW(){
  GPIO_ResetBits(HT_SDA2_P,HT_SDA2);
}

void SCL2_HIGH(){
  GPIO_SetBits(HT_SCK2_P,HT_SCK2);
}

void SCL2_LOW(){
  GPIO_ResetBits(HT_SCK2_P,HT_SCK2);
}

void SDAP_HIGH(){
  GPIO_SetBits(P_SDA_P,P_SDA);
}

#ifdef HardBluetooth
  #define SDA3_HIGH SDAP_HIGH
#else
void SDA3_HIGH(){
  GPIO_SetBits(GPIOA,GPIO_Pin_3);
}
#endif

void SDAP_LOW(){
  GPIO_ResetBits(P_SDA_P,P_SDA);
}

#ifdef HardBluetooth
  #define SDA3_LOW SDAP_LOW
#else
void SDA3_LOW(){
  GPIO_ResetBits(GPIOA,GPIO_Pin_3);
}
#endif

void SCLP_HIGH(){
  GPIO_SetBits(P_SCK_P,P_SCK);
}

#ifdef HardBluetooth
  #define SCL3_HIGH SCLP_HIGH
#else
void SCL3_HIGH(){
  GPIO_SetBits(GPIOA,GPIO_Pin_2);
}
#endif


void SCLP_LOW(){
  GPIO_ResetBits(P_SCK_P,P_SCK);
}

#ifdef HardBluetooth
  #define SCL3_LOW SCLP_LOW
#else
void SCL3_LOW(){
  GPIO_ResetBits(GPIOA,GPIO_Pin_2);
}
#endif


uint8_t read_sdaval1(){
   return GPIO_ReadInputDataBit(HT_SDA1_P,HT_SDA1);
}

uint8_t read_sdaval2(){
   return GPIO_ReadInputDataBit(HT_SDA2_P,HT_SDA2);
}

uint8_t read_sdavalP(){
   return GPIO_ReadInputDataBit(P_SDA_P,P_SDA);
}

#ifdef HardBluetooth
  #define read_sdaval3 read_sdavalP
#else
uint8_t read_sdaval3(){
   return GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_3);
}
#endif


//产生IIC起始信号
void IIC1_Start(void)
{
	SDA1_OUT();     //sda线输出
	SDA1_HIGH();	  	  
	SCL1_HIGH();
	delay_us(4);
 	SDA1_LOW();//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	SCL1_LOW();//钳住I2C总线，准备发送或接收数据 
	delay_us(4);
}	  
void IIC2_Start(void)
{
	SDA2_OUT();     //sda线输出
	SDA2_HIGH();	  	  
	SCL2_HIGH();
	delay_us(4);
 	SDA2_LOW();//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	SCL2_LOW();//钳住I2C总线，准备发送或接收数据 
	delay_us(4);
}	  
void IICP_Start(void)
{
	SDAP_OUT();     //sda线输出
	SDAP_HIGH();	  	  
	SCLP_HIGH();
	delay_us(4);
 	SDAP_LOW();//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	SCLP_LOW();//钳住I2C总线，准备发送或接收数据 
	delay_us(4);
}	

#ifdef HardBluetooth
  #define IIC3_Start IICP_Start
#else
void IIC3_Start(void)
{
	SDA3_OUT();     //sda线输出
	SDA3_HIGH();	  	  
	SCL3_HIGH();
	delay_us(4);
 	SDA3_LOW();//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	SCL3_LOW();//钳住I2C总线，准备发送或接收数据 
	delay_us(4);
}	
#endif

//产生IIC停止信号
void IIC1_Stop(void)
{
	SDA1_OUT();//sda线输出
	SCL1_LOW();
	SDA1_LOW();//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	SCL1_HIGH(); 
	SDA1_HIGH();//发送I2C总线结束信号
	delay_us(4);							   	
}

void IIC2_Stop(void)
{
	SDA2_OUT();//sda线输出
	SCL2_LOW();
	SDA2_LOW();//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	SCL2_HIGH(); 
	SDA2_HIGH();//发送I2C总线结束信号
	delay_us(4);							   	
}

void IICP_Stop(void)
{
	SDAP_OUT();//sda线输出
	SCLP_LOW();
	SDAP_LOW();//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	SCLP_HIGH(); 
	SDAP_HIGH();//发送I2C总线结束信号
	delay_us(4);							   	
}

#ifdef HardBluetooth
  #define IIC3_Stop IICP_Stop
#else
void IIC3_Stop(void)
{
	SDA3_OUT();//sda线输出
	SCL3_LOW();
	SDA3_LOW();//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	SCL3_HIGH(); 
	SDA3_HIGH();//发送I2C总线结束信号
	delay_us(4);							   	
}
#endif

//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
uint8_t IIC1_Wait_Ack(void)
{
	uint8_t ucErrTime=0;
	SDA1_IN();      //SDA设置为输入  
	//SDA1_HIGH();
	//delay_us(1);	   
	SCL1_HIGH();
	delay_us(1);	 
	while(read_sdaval1())
	{
		ucErrTime++;
	//	delay_us(1);
		if(ucErrTime>250)
		{
			IIC1_Stop();
			return 1;
		}
	}
	SCL1_LOW();//时钟输出0 	   
	return 0;  
} 

uint8_t IIC2_Wait_Ack(void)
{
	uint8_t ucErrTime=0;
	SDA2_IN();      //SDA设置为输入  
	//SDA2_HIGH();
	//delay_us(1);	   
	SCL2_HIGH();
	delay_us(1);	 
	while(read_sdaval2())
	{
		ucErrTime++;
	//	delay_us(1);
		if(ucErrTime>250)
		{
			IIC2_Stop();
			return 1;
		}
	}
	SCL2_LOW();//时钟输出0 	   
	return 0;  
} 

uint8_t IICP_Wait_Ack(void)
{
	uint8_t ucErrTime=0;
	SDAP_IN();      //SDA设置为输入  
	//SDAP_HIGH();
	//delay_us(1);	   
	SCLP_HIGH();
	delay_us(1);	 
	while(read_sdavalP())
	{
		ucErrTime++;
	//	delay_us(1);
		if(ucErrTime>250)
		{
			IICP_Stop();
			return 1;
		}
	}
	SCLP_LOW();//时钟输出0 	   
	return 0;  
} 

#ifdef HardBluetooth
  #define IIC3_Wait_Ack IICP_Wait_Ack
#else
uint8_t IIC3_Wait_Ack(void)
{
	uint8_t ucErrTime=0;
	SDA3_IN();      //SDA设置为输入  
	//SDA3_HIGH();
	//delay_us(1);	   
	SCL3_HIGH();
	delay_us(1);	 
	while(read_sdaval3())
	{
		ucErrTime++;
	//	delay_us(1);
		if(ucErrTime>250)
		{
			IIC3_Stop();
			return 1;
		}
	}
	SCL3_LOW();//时钟输出0 	   
	return 0;  
}
#endif

//产生ACK应答
void IIC1_Ack(void)
{
	SCL1_LOW();
	SDA1_OUT();
	SDA1_LOW();
	delay_us(20);
	SCL1_HIGH();
	delay_us(2);
	SCL1_LOW();
}

void IIC2_Ack(void)
{
	SCL2_LOW();
	SDA2_OUT();
	SDA2_LOW();
	delay_us(20);
	SCL2_HIGH();
	delay_us(2);
	SCL2_LOW();
}

void IICP_Ack(void)
{
	SCLP_LOW();
	SDAP_OUT();
	SDAP_LOW();
	delay_us(20);
	SCLP_HIGH();
	delay_us(2);
	SCLP_LOW();
}

#ifdef HardBluetooth
  #define IIC3_Ack IICP_Ack
#else
void IIC3_Ack(void)
{
	SCL3_LOW();
	SDA3_OUT();
	SDA3_LOW();
	delay_us(20);
	SCL3_HIGH();
	delay_us(2);
	SCL3_LOW();
}
#endif


//不产生ACK应答		    
void IIC1_NAck(void)
{
	SCL1_LOW();
	SDA1_OUT();
	SDA1_HIGH();
	delay_us(5);
	SCL1_HIGH();
	delay_us(5);
	SCL1_LOW();
}	

void IIC2_NAck(void)
{
	SCL2_LOW();
	SDA2_OUT();
	SDA2_HIGH();
	delay_us(5);
	SCL2_HIGH();
	delay_us(5);
	SCL2_LOW();
}	

void IICP_NAck(void)
{
	SCLP_LOW();
	SDAP_OUT();
	SDAP_HIGH();
	delay_us(5);
	SCLP_HIGH();
	delay_us(5);
	SCLP_LOW();
}

#ifdef HardBluetooth
  #define IIC3_NAck IICP_NAck
#else
void IIC3_NAck(void)
{
	SCL3_LOW();
	SDA3_OUT();
	SDA3_HIGH();
	delay_us(5);
	SCL3_HIGH();
	delay_us(5);
	SCL3_LOW();
}
#endif

//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void IIC1_Send_Byte(uint8_t txd)
{                        
    uint8_t t;   
	  SDA1_OUT(); 	    
    SCL1_LOW();//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
			  if((txd&0x80)>>7){
				  SDA1_HIGH();
        }
				else{
				  SDA1_LOW();
        }
        txd<<=1; 	  
			  delay_us(5);   //对TEA5767这三个延时都是必须的
			  SCL1_HIGH();
			  delay_us(5); 
			  SCL1_LOW();	
			  delay_us(5);
    }	 
} 	    

void IIC2_Send_Byte(uint8_t txd)
{                        
    uint8_t t;   
	  SDA2_OUT(); 	    
    SCL2_LOW();//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
			  if((txd&0x80)>>7){
				  SDA2_HIGH();
        }
				else{
				  SDA2_LOW();
        }
        txd<<=1; 	  
			  delay_us(5);   //对TEA5767这三个延时都是必须的
			  SCL2_HIGH();
			  delay_us(5); 
			  SCL2_LOW();	
			  delay_us(5);
    }	 
} 	    

void IICP_Send_Byte(uint8_t txd)
{                        
    uint8_t t;   
	  SDAP_OUT(); 	    
    SCLP_LOW();//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
			  if((txd&0x80)>>7){
				  SDAP_HIGH();
        }
				else{
				  SDAP_LOW();
        }
        txd<<=1; 	  
			  delay_us(5);   //对TEA5767这三个延时都是必须的
			  SCLP_HIGH();
			  delay_us(5); 
			  SCLP_LOW();	
			  delay_us(5);
    }	 
} 	    

#ifdef HardBluetooth
  #define IIC3_Send_Byte IICP_Send_Byte
#else
void IIC3_Send_Byte(uint8_t txd)
{                        
    uint8_t t;   
	  SDA3_OUT(); 	    
    SCL3_LOW();//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
			  if((txd&0x80)>>7){
				  SDA3_HIGH();
        }
				else{
				  SDA3_LOW();
        }
        txd<<=1; 	  
			  delay_us(5);   //对TEA5767这三个延时都是必须的
			  SCL3_HIGH();
			  delay_us(5); 
			  SCL3_LOW();	
			  delay_us(5);
    }	 
} 	    
#endif

//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
uint8_t IIC1_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA1_IN();//SDA设置为输入
    for(i=0;i<8;i++ )
	{
    SCL1_LOW(); 
    delay_us(5);
		SCL1_HIGH();
    receive<<=1;
    if(read_sdaval1()>0)receive++;   
		delay_us(5); 
    }					 
    if (!ack)
        IIC1_NAck();//发送nACK
    else
        IIC1_Ack(); //发送ACK   
    return receive;
}

uint8_t IIC2_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA2_IN();//SDA设置为输入
    for(i=0;i<8;i++ )
	{
    SCL2_LOW(); 
    delay_us(5);
		SCL2_HIGH();
    receive<<=1;
    if(read_sdaval2()>0)receive++;   
		delay_us(5); 
    }					 
    if (!ack)
        IIC2_NAck();//发送nACK
    else
        IIC2_Ack(); //发送ACK   
    return receive;
}

uint8_t IICP_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDAP_IN();//SDA设置为输入
    for(i=0;i<8;i++ )
	{
    SCLP_LOW(); 
    delay_us(5);
		SCLP_HIGH();
    receive<<=1;
    if(read_sdavalP()>0)receive++;   
		delay_us(5); 
    }					 
    if (!ack)
        IICP_NAck();//发送nACK
    else
        IICP_Ack(); //发送ACK   
    return receive;
}

#ifdef HardBluetooth
  #define IIC3_Read_Byte IICP_Read_Byte
#else
uint8_t IIC3_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA3_IN();//SDA设置为输入
    for(i=0;i<8;i++ )
	{
    SCL3_LOW(); 
    delay_us(5);
		SCL3_HIGH();
    receive<<=1;
    if(read_sdaval3()>0)receive++;   
		delay_us(5); 
    }					 
    if (!ack)
        IIC3_NAck();//发送nACK
    else
        IIC3_Ack(); //发送ACK   
    return receive;
}
#endif

//========================SHT2x(HTU21D)=======================

SHT2x_data SHT20;

void EnableSHT2(void)
{
	GPIO_InitTypeDef    GPIO_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);	
	GPIO_InitStructure.GPIO_Pin = HT_VDD;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//push-pull
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(HT_VDD_P,&GPIO_InitStructure);
  GPIO_SetBits(HT_VDD_P,HT_VDD);
}

#ifndef HardBluetooth
void EnableSHT3(void)
{
	GPIO_InitTypeDef    GPIO_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//push-pull
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
  GPIO_SetBits(GPIOB,GPIO_Pin_1);
}
#endif

void DisableSHT2(void)
{
	//芯片使能初始化
	GPIO_InitTypeDef    GPIO_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);	
	GPIO_InitStructure.GPIO_Pin = HT_VDD;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//push-pull
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(HT_VDD_P,&GPIO_InitStructure);
	GPIO_ResetBits(HT_VDD_P,HT_VDD);
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);	
	GPIO_InitStructure.GPIO_Pin = HT_SDA1|HT_SDA2|HT_SCK1|HT_SCK2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//push-pull
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(HT_SDA1_P,&GPIO_InitStructure);

	GPIO_ResetBits(HT_VDD_P,HT_VDD);
	GPIO_ResetBits(HT_SDA1_P,HT_SDA1);
	GPIO_ResetBits(HT_SDA2_P,HT_SDA2);
	GPIO_ResetBits(HT_SCK1_P,HT_SCK1);
	GPIO_ResetBits(HT_SCK2_P,HT_SCK2);
}

#ifndef HardBluetooth
void DisableSHT3(void)
{
	//芯片使能初始化
	GPIO_InitTypeDef    GPIO_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//push-pull
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	GPIO_ResetBits(GPIOB,GPIO_Pin_1);
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//push-pull
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	GPIO_ResetBits(GPIOA,GPIO_Pin_2);
	GPIO_ResetBits(GPIOA,GPIO_Pin_3);
}
#endif

void SHT2x_Init(void)
{
	IIC1_Init();
	IIC2_Init();
	IICP_Init();
#ifndef HardBluetooth
	IIC3_Init();
#endif	
}

uint8_t SHT2x_SoftReset1(void) //SHT20软件复位
{
	uint8_t err=0;
	IIC1_Start();
	IIC1_Send_Byte(I2C_ADR_W);
	err = IIC1_Wait_Ack();
	IIC1_Send_Byte(0xFE);
	err = IIC1_Wait_Ack();
	IIC1_Stop();
  delay_ms(10);               
	return err;
}

uint8_t SHT2x_SoftReset2(void) //SHT20软件复位
{
	uint8_t err=0;
	IIC2_Start();
	IIC2_Send_Byte(I2C_ADR_W);
	err = IIC2_Wait_Ack();
	IIC2_Send_Byte(0xFE);
	err = IIC2_Wait_Ack();
	IIC2_Stop();
  delay_ms(10);               
	return err;
}

uint8_t SHT2x_SoftReset3(void) //SHT20软件复位
{
	uint8_t err=0;
	IIC3_Start();
	IIC3_Send_Byte(I2C_ADR_W);
	err = IIC3_Wait_Ack();
	IIC3_Send_Byte(0xFE);
	err = IIC3_Wait_Ack();
	IIC3_Stop();
  delay_ms(10);               
	return err;
}

float SHT2x_GetTempPoll1()
{
    float TEMP;
    uint8_t ack, tmp1, tmp2;
//    uint16_t ST;
    uint16_t i=0;
	
    IIC1_Start();				//发送IIC开始信号
    IIC1_Send_Byte(I2C_ADR_W);			//IIC发送一个字节 
		ack = IIC1_Wait_Ack();	
		IIC1_Send_Byte(TRIG_TEMP_MEASUREMENT_POLL);
		ack = IIC1_Wait_Ack();

    do {
        delay_ms(10);               
        IIC1_Start();				//发送IIC开始信号
        IIC1_Send_Byte(I2C_ADR_R);	
			  i++;
			  ack = IIC1_Wait_Ack();
			  if(i==5)break;
    } while(ack!=0);
		
    tmp1 = IIC1_Read_Byte(1);
    tmp2 = IIC1_Read_Byte(1);
		IIC1_Read_Byte(0);
    IIC1_Stop();
		TEMP=tmp1*256+tmp2;
		TEMP=(175.72)*TEMP/65536-46.85;
		/*
    ST = (tmp1 << 8) | (tmp2 << 0);
    ST &= ~0x0003;		
    TEMP = ((float)ST * 0.00268127) - 46.85;
		*/
    return (TEMP);	  
}

float SHT2x_GetTempPoll2()
{
    float TEMP;
    uint8_t ack, tmp1, tmp2;
//    uint16_t ST;
    uint16_t i=0;
	
    IIC2_Start();				//发送IIC开始信号
    IIC2_Send_Byte(I2C_ADR_W);			//IIC发送一个字节 
		ack = IIC2_Wait_Ack();	
		IIC2_Send_Byte(TRIG_TEMP_MEASUREMENT_POLL);
		ack = IIC2_Wait_Ack();

    do {
        delay_ms(10);               
        IIC2_Start();				//发送IIC开始信号
        IIC2_Send_Byte(I2C_ADR_R);	
			  i++;
			  ack = IIC2_Wait_Ack();
			  if(i==5)break;
    } while(ack!=0);
		
    tmp1 = IIC2_Read_Byte(1);
    tmp2 = IIC2_Read_Byte(1);
		IIC2_Read_Byte(0);
    IIC2_Stop();
		TEMP=tmp1*256+tmp2;
		TEMP=(175.72)*TEMP/65536-46.85;
		/*
    ST = (tmp1 << 8) | (tmp2 << 0);
    ST &= ~0x0003;		
    TEMP = ((float)ST * 0.00268127) - 46.85;
		*/
    return (TEMP);	  
}

float SHT2x_GetTempPoll3()
{
    float TEMP;
    uint8_t ack, tmp1, tmp2;
//    uint16_t ST;
    uint16_t i=0;
	
    IIC3_Start();				//发送IIC开始信号
    IIC3_Send_Byte(I2C_ADR_W);			//IIC发送一个字节 
		ack = IIC3_Wait_Ack();	
		IIC3_Send_Byte(TRIG_TEMP_MEASUREMENT_POLL);
		ack = IIC3_Wait_Ack();

    do {
        delay_ms(10);               
        IIC3_Start();				//发送IIC开始信号
        IIC3_Send_Byte(I2C_ADR_R);	
			  i++;
			  ack = IIC3_Wait_Ack();
			  if(i==5)break;
    } while(ack!=0);
		
    tmp1 = IIC3_Read_Byte(1);
    tmp2 = IIC3_Read_Byte(1);
		IIC3_Read_Byte(0);
    IIC3_Stop();
		TEMP=tmp1*256+tmp2;
		TEMP=(175.72)*TEMP/65536-46.85;
		/*
    ST = (tmp1 << 8) | (tmp2 << 0);
    ST &= ~0x0003;		
    TEMP = ((float)ST * 0.00268127) - 46.85;
		*/
    return (TEMP);	  
}

float SHT2x_GetHumiPoll1()
{
    float HUMI;
    uint8_t ack, tmp1, tmp2;
//    uint16_t SRH;
    uint16_t i=0;
    IIC1_Start();				//发送IIC开始信号
    IIC1_Send_Byte(I2C_ADR_W);			//IIC发送一个字节 
    ack = IIC1_Wait_Ack();	
    IIC1_Send_Byte(TRIG_HUMI_MEASUREMENT_POLL);
    ack = IIC1_Wait_Ack(); 
    do {
        delay_ms(10);               
        IIC1_Start();				//发送IIC开始信号
        IIC1_Send_Byte(I2C_ADR_R);	
        i++;
        ack = IIC1_Wait_Ack();
        if(i==5)break;
    } while(ack!=0);
    
    tmp1 = IIC1_Read_Byte(1); 
    tmp2 = IIC1_Read_Byte(1);
    IIC1_Read_Byte(0);
    IIC1_Stop();
	
		HUMI=tmp1*256+tmp2;
		HUMI=(125)*HUMI/65536-6;
		/*
    SRH = (tmp1 << 8) | (tmp2 << 0);
    SRH &= ~0x0003;
		
    HUMI = ((float)SRH * 0.00190735) - 6;
		*/
    return (HUMI);
}

float SHT2x_GetHumiPoll2()
{
    float HUMI;
    uint8_t ack, tmp1, tmp2;
//    uint16_t SRH;
    uint16_t i=0;
    IIC2_Start();				//发送IIC开始信号
    IIC2_Send_Byte(I2C_ADR_W);			//IIC发送一个字节 
    ack = IIC2_Wait_Ack();	
    IIC2_Send_Byte(TRIG_HUMI_MEASUREMENT_POLL);
    ack = IIC2_Wait_Ack(); 
    do {
        delay_ms(10);               
        IIC2_Start();				//发送IIC开始信号
        IIC2_Send_Byte(I2C_ADR_R);	
        i++;
        ack = IIC2_Wait_Ack();
        if(i==5)break;
    } while(ack!=0);
    
    tmp1 = IIC2_Read_Byte(1); 
    tmp2 = IIC2_Read_Byte(1);
    IIC2_Read_Byte(0);
    IIC2_Stop();
		
		HUMI=tmp1*256+tmp2;
		HUMI=(125)*HUMI/65536-6;
		/*
    SRH = (tmp1 << 8) | (tmp2 << 0);
    SRH &= ~0x0003;
		
    HUMI = ((float)SRH * 0.00190735) - 6;
		*/
    return (HUMI);
}

float SHT2x_GetHumiPoll3()
{
    float HUMI;
    uint8_t ack, tmp1, tmp2;
//    uint16_t SRH;
    uint16_t i=0;
    IIC3_Start();				//发送IIC开始信号
    IIC3_Send_Byte(I2C_ADR_W);			//IIC发送一个字节 
    ack = IIC3_Wait_Ack();	
    IIC3_Send_Byte(TRIG_HUMI_MEASUREMENT_POLL);
    ack = IIC3_Wait_Ack(); 
    do {
        delay_ms(10);               
        IIC3_Start();				//发送IIC开始信号
        IIC3_Send_Byte(I2C_ADR_R);	
        i++;
        ack = IIC3_Wait_Ack();
        if(i==5)break;
    } while(ack!=0);
    
    tmp1 = IIC3_Read_Byte(1); 
    tmp2 = IIC3_Read_Byte(1);
    IIC3_Read_Byte(0);
    IIC3_Stop();
		
		HUMI=tmp1*256+tmp2;
		HUMI=(125)*HUMI/65536-6;
		/*
    SRH = (tmp1 << 8) | (tmp2 << 0);
    SRH &= ~0x0003;
		
    HUMI = ((float)SRH * 0.00190735) - 6;
		*/
    return (HUMI);
}

/****************** pressure ms5637 *************************/
#define  MS5637_I2C_ADR_W 0xEC 
#define  MS5637_I2C_ADR_R 0xED  

MS5637Info ms5637Info;

void DisableMS5637()
{
	GPIO_ResetBits(P_VDD_P, P_VDD);
	GPIO_ResetBits(P_SDA_P, P_SDA);
	GPIO_ResetBits(P_SCK_P, P_SCK);
}

void EnableMS5637()
{
    GPIO_InitTypeDef    GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = P_VDD;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//push-pull
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//
    GPIO_Init(GPIOB,&GPIO_InitStructure);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);	
    GPIO_SetBits(P_VDD_P,P_VDD);
}

/*
static void IIC_WriteByte(uint8_t cmd)
{
		volatile uint8_t ack;
    IICP_Start();			
    IICP_Send_Byte(MS5637_I2C_ADR_W);			
    ack = IICP_Wait_Ack();	
    IICP_Send_Byte(cmd);
    ack = IICP_Wait_Ack();
    IICP_Stop();
}
*/

void IIC_ReadBytes(uint8_t reg, uint8_t len, uint8_t *rData)
{
	uint8_t i = 0;
	uint8_t ack;
	IICP_Start();			
	IICP_Send_Byte(MS5637_I2C_ADR_W);			
	ack = IICP_Wait_Ack();	
	IICP_Send_Byte(reg);
	ack = IICP_Wait_Ack();
	IICP_Stop();
    
    do {
        delay_ms(10);               
        IICP_Start();				//发送IIC开始信号
        IICP_Send_Byte(MS5637_I2C_ADR_R);	
        i++;
        ack = IICP_Wait_Ack();
        if(i==5)break;
    } while(ack!=0);
#if 1
    for(i=0; i<len-1; i++)
    {
        *rData++ = IICP_Read_Byte(1);
    }
    if(len > 0)
    {
        *rData = IICP_Read_Byte(0);
    }
    #else
     for(i=0; i<len-1; i++)
    {
        *rData++ = IICP_Read_Byte(1);
    }
#endif    
    IICP_Stop();
}

uint8_t MS5637_SoftReset(void) //MS56370软件复位
{
    uint8_t err=0;
    IICP_Start();
    IICP_Send_Byte(MS5637_I2C_ADR_W);
    err = IICP_Wait_Ack();
    IICP_Send_Byte(0x1E);
    err = IICP_Wait_Ack();
    IICP_Stop();
    return err;
}

void i2c_master_init(void)
{
	IICP_Init();
}

enum status_code i2c_master_write_packet_wait(struct i2c_master_packet *packet)
{
	 uint8_t ack;
    IICP_Start();			
    IICP_Send_Byte(((packet->address)<<1)&0xFE);			
    ack = IICP_Wait_Ack();	
	if(ack != 0)
	{
		return STATUS_ERR_OVERFLOW;
	}
    IICP_Send_Byte(packet->data[0]);
    ack = IICP_Wait_Ack();
    IICP_Stop();	
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
		IICP_Start();				//发送IIC开始信号
		IICP_Send_Byte(((packet->address)<<1)|0x01);	
		i++;
		ack = IICP_Wait_Ack();
		if(i==5)break;
	} while(ack!=0);
	#if 1
	for(i=0; i<packet->data_length-1; i++)
	{
		packet->data[i] = IICP_Read_Byte(1);
	}
	if(packet->data_length > 0)
	{
		packet->data[packet->data_length-1] = IICP_Read_Byte(0);
	}
	#else
	 for(i=0; i<packet->data_length-1; i++)
	{
		packet->data[i] = IICP_Read_Byte(1);
	}
	#endif    
	IICP_Stop();
    delay_ms(10);
	if(1==ack)
	{
		return STATUS_ERR_OVERFLOW;
	}
	return STATUS_OK;
}

void GetHumiTemp1(float *humi, float *temp)
{
    uint8_t i = 0;
    uint8_t validCnt = 0;
    float val = 0;
    float sum = 0;

    i = 0;
		TimeOutSign=0;
    do
    {
        i++;
        if(i == 20)
        {
            break;
        }
    }
		while ((SHT2x_SoftReset1())&&Time_Out_Break(50,&TimeOutSign)==0);//超时5秒
		if (Time_Out_Break(50,&TimeOutSign)==1) {*humi=0;*temp=0;return;}
    validCnt = 0;
    sum = 0;
    for(i=0; i<10; i++)
    {
        val = SHT2x_GetHumiPoll1();
        if(0 < val && val < 100)
        {
            validCnt++;
            sum += val;
        }
    }
    if(0 != validCnt)
    {
        *humi = sum/validCnt;
    }

    validCnt = 0;
    sum = 0;
    for(i=0; i<10; i++)
    {
        val = SHT2x_GetTempPoll1();
        if(-40 < val && val < 125)
        {
            validCnt++;
            sum += val;
        }
    }
    if(0 != validCnt)
    {
        *temp = sum/validCnt;
    }
}

void GetHumiTemp2(float *humi, float *temp)
{
    uint8_t i = 0;
    uint8_t validCnt = 0;
    float val = 0;
    float sum = 0;

    i = 0;
		TimeOutSign=0;
    do
    {
        i++;
        if(i == 20)
        {
            break;
        }
    }
		while ((SHT2x_SoftReset2())&&Time_Out_Break(50,&TimeOutSign)==0);//超时5秒
		if (Time_Out_Break(50,&TimeOutSign)==1) {*humi=0;*temp=0;return;}
    validCnt = 0;
    sum = 0;
    for(i=0; i<10; i++)
    {
        val = SHT2x_GetHumiPoll2();
        if(0 < val && val < 100)
        {
            validCnt++;
            sum += val;
        }
    }
    if(0 != validCnt)
    {
        *humi = sum/validCnt;
    }

    validCnt = 0;
    sum = 0;
    for(i=0; i<10; i++)
    {
        val = SHT2x_GetTempPoll2();
        if(-40 < val && val < 125)
        {
            validCnt++;
            sum += val;
        }
    }
    if(0 != validCnt)
    {
        *temp = sum/validCnt;
    }
}

void GetHumiTemp3(float *humi, float *temp)
{
    uint8_t i = 0;
    uint8_t validCnt = 0;
    float val = 0;
    float sum = 0;

    i = 0;
		TimeOutSign=0;
    do
    {
        i++;
        if(i == 20)
        {
            break;
        }
    }
		while ((SHT2x_SoftReset3())&&Time_Out_Break(50,&TimeOutSign)==0);//超时5秒
		if (Time_Out_Break(50,&TimeOutSign)==1) {*humi=0;*temp=0;return;}
    validCnt = 0;
    sum = 0;
    for(i=0; i<10; i++)
    {
        val = SHT2x_GetHumiPoll3();
        if(0 < val && val < 100)
        {
            validCnt++;
            sum += val;
        }
    }
    if(0 != validCnt)
    {
        *humi = sum/validCnt;
    }

    validCnt = 0;
    sum = 0;
    for(i=0; i<10; i++)
    {
        val = SHT2x_GetTempPoll3();
        if(-40 < val && val < 125)
        {
            validCnt++;
            sum += val;
        }
    }
    if(0 != validCnt)
    {
        *temp = sum/validCnt;
    }
}

