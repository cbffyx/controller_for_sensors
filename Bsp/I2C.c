#include "my_type.h"


static etError I2c_WaitWhileClockStreching(uint8_t timeout);
//-----------------------------------------------------------------------------
void I2C_Config(void) 
{
    I2C_SCL_OUT;
    I2C_SDA_OUT;
}

//-----------------------------------------------------------------------------
void I2c_StartCondition()
{
    I2C_SDA_OUT;                //配置SDA引脚为推挽输出模式
      
    I2C_SDA_High;               //SDA引脚置为高电平
    delay_us(2);                //延时等待
    I2C_SCL_High;               //SCL引脚置为高电平
    delay_us(2);                //延时等待
    I2C_SDA_Low;                //将SDA置低产生下降沿（产生起始信号）
    delay_us(2);                //延时等待
    I2C_SCL_Low;                //将SCL置低产生下降沿（允许SDA数据传送）
    delay_us(2);                //延时等待
}

//-----------------------------------------------------------------------------
void I2c_StopCondition()
{
    I2C_SDA_OUT;                //配置SDA引脚为推挽输出模式
    
    I2C_SDA_Low;                //SDA引脚置为低电平
    delay_us(2);                //延时等待
    I2C_SCL_High;               //将SCL引脚置高产生上升沿
    delay_us(2);                //延时等待
    I2C_SDA_High;               //将SDA引脚置高产生上升沿（产生终止信号）
    delay_us(2);                //延时等待
}

//-----------------------------------------------------------------------------
void I2c_SendACK(etI2cAck ack)
{
    I2C_SDA_OUT;                //配置SDA引脚为推挽输出模式
    
    if(ack == ACK) I2C_SDA_Low; //send acknowledge if necessary
    else           I2C_SDA_High;
	delay_us(2);               	//延时
    I2C_SCL_High;               //将SCL引脚置高产生上升沿
    delay_us(2);               	//延时
    I2C_SCL_Low;                //将SCL置低产生下降沿（允许SDA数据传送）
    delay_us(2);               	//延时
}

//-----------------------------------------------------------------------------
bt I2c_RecvACK(void)
{
    bt bits = FALSE;
    
    I2C_SCL_High;               //拉高时钟线
    delay_us(2);                //延时
    I2C_SDA_IN;                 //配置SDA引脚为推挽输入模式  
    if(1 == get_SDA()) bits = TRUE;
    else               bits = FALSE;
    
    I2C_SCL_Low;                //拉低时钟线
    delay_us(2);

    return bits;
}

//-----------------------------------------------------------------------------
etError I2c_WriteByte(uint8_t txByte)
{
    etError error = NO_ERROR;
    uint8_t mask;

    I2C_SDA_OUT;                //配置SDA引脚为推挽输出模式  
    for(mask = 0x80; mask > 0; mask>>=1)//shift bit for masking (8 times)
 	{
		if((mask & txByte) == 0) I2C_SDA_Low;
		else					 I2C_SDA_High;
		delay_us(1);
		I2C_SCL_High;           //拉高时钟线
        delay_us(4);           	//延时
        I2C_SCL_Low;            //拉低时钟线
        delay_us(4);           	//延时
	}
    if(TRUE == I2c_RecvACK()) error = ACK_ERROR; 
    return error;               // return error code
}

//-----------------------------------------------------------------------------
etError I2c_ReadByte(uint8_t *rxByte, etI2cAck ack, uint8_t timeout)
{
    etError error = NO_ERROR;
    uint8_t mask;
    *rxByte = 0x00;
    
    I2C_SDA_IN;                 //配置SDA引脚为推挽输入模式 
    for(mask = 0x80; mask > 0; mask >>= 1)//shift bit for masking (8 times)
    { 
        I2C_SCL_High;           // start clock on SCL-line
        delay_us(4);            // clock set-up time (t_SU;CLK)
        error = I2c_WaitWhileClockStreching(timeout);// wait while clock streching
        delay_us(1);
        if(1 == get_SDA()) *rxByte |= mask; // read bit
        I2C_SCL_Low;
        delay_us(4);            // data hold time(t_HD;DAT)
    }
    if(ack == ACK) I2c_SendACK(ACK);// send acknowledge if necessary
 	else 		   I2c_SendACK(NACK);
  
    return error;               // return with no error
}

//-----------------------------------------------------------------------------
static etError I2c_WaitWhileClockStreching(uint8_t timeout)
{
    etError error = NO_ERROR;
    
    I2C_SCL_IN;
    while(0 == get_SCL())
    {
        if(timeout-- == 0) return TIMEOUT_ERROR;
        delay_ms(1);
    }
    I2C_SCL_OUT;
    return error;
}

//-----------------------------------------------------------------------------
etError Device_StartWriteAccess(uint8_t i2cAddress,uint8_t bits)
{
	etError error; // error code

	// write a start condition
	I2c_StartCondition();
	// write the sensor I2C address with the write flag
    
    if(bits == 7)
        error = I2c_WriteByte(i2cAddress << 1);
    if(bits == 8)
        error = I2c_WriteByte(i2cAddress);
	return error;
}

//-----------------------------------------------------------------------------
etError Device_StartReadAccess(uint8_t i2cAddress,uint8_t bits)
{
	etError error; // error code

	// write a start condition
	I2c_StartCondition();
	// write the sensor I2C address with the read flag
    
    if(bits == 7)
        error = I2c_WriteByte(i2cAddress << 1 | 0x01);   
    if(bits == 8)
        error = I2c_WriteByte(i2cAddress+1);  
	return error;
}

//-----------------------------------------------------------------------------
void Device_StopAccess(void)
{
 	// write a stop condition
	I2c_StopCondition();
}

//-----------------------------------------------------------------------------
etError I2C_WriteNbyte(uint8_t Device,uint8_t bits,uint8_t addr,uint8_t *p,uint8_t number)
{
    etError error; // error code
    
    error  = Device_StartWriteAccess(Device,bits);
    error |= I2c_WriteByte(addr);
    // if no error, start write access
    if(error == NO_ERROR) 
    {
        do
        {
            error = I2c_WriteByte(*p++);
            if(error != NO_ERROR) break;
        }while(--number);
    }
    Device_StopAccess();     
    return error;
}

//-----------------------------------------------------------------------------
etError I2C_ReadNbyte(uint8_t Device,uint8_t bits,uint8_t addr,uint8_t *p,uint8_t number)
{
    etError error; // error code
    
    error  = Device_StartWriteAccess(Device,bits);
    error |= I2c_WriteByte(addr);
    // if no error, start read access
    if(error == NO_ERROR) error = Device_StartReadAccess(Device,bits);
    // if no error, read first serial number word   
    if(error == NO_ERROR) 
    {
        do
        {
            if(number==1) 
            {
                I2c_ReadByte(p, NACK, 0); break;
            }
            error = I2c_ReadByte(p++, ACK, 100);
            if(error != NO_ERROR) break;  
        }while(--number);
    }  
    Device_StopAccess();      
    return error;
}
