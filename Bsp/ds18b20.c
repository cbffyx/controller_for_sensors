#include "my_type.h"


//-----------------------------------------------------------------------------
void DS18B20_Reset()
{ 
	uint8_t ErrorTime=10;
    uint8_t CY = 1;
    
    while (CY)
    {
        DS18B20_DQ_OUT;             //改变DQ引脚方向性为输出方式
        DS18B20_DQ_Low;             //送出低电平复位信号
        delay_us(240);              //延时至少480us
		delay_us(240);
        DS18B20_DQ_High;            //释放数据线
        delay_us(60);               //等待60us
        DS18B20_DQ_IN;              //改变DQ引脚方向性为输入方式
        CY = GPIO_ReadInputDataBit(DQ_PORT,DQ_PINS);   
		ErrorTime--;
		if(ErrorTime == 0) break;
        delay_us(240);              //等待设备释放数据线
		delay_us(180);
    }
}

//-----------------------------------------------------------------------------
uint8_t DS18B20_ReadByte()
{
    uint8_t i;
    uint8_t dat = 0;
    for (i=0; i<8; i++)             //8位计数器
    {
        dat >>= 1;
        DS18B20_DQ_OUT;             //改变DQ引脚方向性为输出方式
        DS18B20_DQ_Low;             //DQ单总线拉低
        delay_us(2);                //延时等待
        DS18B20_DQ_High;            //准备接收
        delay_us(2);                //接收延时
        DS18B20_DQ_IN;              //改变DQ引脚方向性为输入方式
        if(1 == get_DQ()) dat |= 0x80; 
        delay_us(60);               //等待时间片结束
    }
    return dat;
}

//-----------------------------------------------------------------------------
void DS18B20_WriteByte(uchar dat)
{
    uint8_t i;
    DS18B20_DQ_OUT;                 //改变DQ引脚方向性为输出方式
    for (i=0; i<8; i++)             //8位计数器
    {
        DS18B20_DQ_Low;             //DQ单总线拉低
        delay_us(2);                //延时等待

        if((dat&0x01)==1) DS18B20_DQ_High;
        else              DS18B20_DQ_Low;

        delay_us(60);               //等待时间片结束
        DS18B20_DQ_High;            //恢复数据线
        dat >>= 1;                  //送出数据
    }
}

//-----------------------------------------------------------------------------
float DS18B20()
{
	uint8_t  TL=0,TH=10;
	uint16_t temp=0;

	DS18B20_Reset();
	DS18B20_WriteByte(0xcc);
	DS18B20_WriteByte(0x44);
    
    DS18B20_DQ_IN;                  //改变DQ引脚方向性为输入方式
    delay_ms(2);  
    while(0 == get_DQ())
    {
        delay_ms(1);      
        if(--TH == 0) return 65535;    
    }

	DS18B20_Reset();
	DS18B20_WriteByte(0xcc);
	DS18B20_WriteByte(0xbe);

	TL = DS18B20_ReadByte();        //读温度低字节
    TH = DS18B20_ReadByte();        //读温度高字节
	
	((uchar *)&temp)[1] = TL;
	((uchar *)&temp)[0] = TH;
    
    if(temp == 0xffff) return 65535;
	if(temp&0x8000)					//负温度 
	{  
		temp = (~temp) + 1;
        return ((float)temp*0.0625*(-1)*0.9);
	}
    else
    {
        return ((float)temp*0.0625*0.9);
    }
}