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
    I2C_SDA_OUT;                //����SDA����Ϊ�������ģʽ
      
    I2C_SDA_High;               //SDA������Ϊ�ߵ�ƽ
    delay_us(2);                //��ʱ�ȴ�
    I2C_SCL_High;               //SCL������Ϊ�ߵ�ƽ
    delay_us(2);                //��ʱ�ȴ�
    I2C_SDA_Low;                //��SDA�õͲ����½��أ�������ʼ�źţ�
    delay_us(2);                //��ʱ�ȴ�
    I2C_SCL_Low;                //��SCL�õͲ����½��أ�����SDA���ݴ��ͣ�
    delay_us(2);                //��ʱ�ȴ�
}

//-----------------------------------------------------------------------------
void I2c_StopCondition()
{
    I2C_SDA_OUT;                //����SDA����Ϊ�������ģʽ
    
    I2C_SDA_Low;                //SDA������Ϊ�͵�ƽ
    delay_us(2);                //��ʱ�ȴ�
    I2C_SCL_High;               //��SCL�����ø߲���������
    delay_us(2);                //��ʱ�ȴ�
    I2C_SDA_High;               //��SDA�����ø߲��������أ�������ֹ�źţ�
    delay_us(2);                //��ʱ�ȴ�
}

//-----------------------------------------------------------------------------
void I2c_SendACK(etI2cAck ack)
{
    I2C_SDA_OUT;                //����SDA����Ϊ�������ģʽ
    
    if(ack == ACK) I2C_SDA_Low; //send acknowledge if necessary
    else           I2C_SDA_High;
	delay_us(2);               	//��ʱ
    I2C_SCL_High;               //��SCL�����ø߲���������
    delay_us(2);               	//��ʱ
    I2C_SCL_Low;                //��SCL�õͲ����½��أ�����SDA���ݴ��ͣ�
    delay_us(2);               	//��ʱ
}

//-----------------------------------------------------------------------------
bt I2c_RecvACK(void)
{
    bt bits = FALSE;
    
    I2C_SCL_High;               //����ʱ����
    delay_us(2);                //��ʱ
    I2C_SDA_IN;                 //����SDA����Ϊ��������ģʽ  
    if(1 == get_SDA()) bits = TRUE;
    else               bits = FALSE;
    
    I2C_SCL_Low;                //����ʱ����
    delay_us(2);

    return bits;
}

//-----------------------------------------------------------------------------
etError I2c_WriteByte(uint8_t txByte)
{
    etError error = NO_ERROR;
    uint8_t mask;

    I2C_SDA_OUT;                //����SDA����Ϊ�������ģʽ  
    for(mask = 0x80; mask > 0; mask>>=1)//shift bit for masking (8 times)
 	{
		if((mask & txByte) == 0) I2C_SDA_Low;
		else					 I2C_SDA_High;
		delay_us(1);
		I2C_SCL_High;           //����ʱ����
        delay_us(4);           	//��ʱ
        I2C_SCL_Low;            //����ʱ����
        delay_us(4);           	//��ʱ
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
    
    I2C_SDA_IN;                 //����SDA����Ϊ��������ģʽ 
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
