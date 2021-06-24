#ifndef _I2C_H_
#define _I2C_H_


//-----------------------------------------------------------------------------
#define I2C_SCL_OUT  GPIO_Init(SCL_PORT, SCL_PINS, GPIO_Mode_Out_PP_High_Slow)
#define I2C_SDA_OUT  GPIO_Init(SDA_PORT, SDA_PINS, GPIO_Mode_Out_PP_High_Slow)
#define I2C_SDA_IN   GPIO_Init(SDA_PORT, SDA_PINS, GPIO_Mode_In_PU_No_IT)
#define I2C_SCL_IN   GPIO_Init(SCL_PORT, SCL_PINS, GPIO_Mode_In_PU_No_IT)

#define I2C_SDA_High GPIO_WriteBit(SDA_PORT,SDA_PINS,SET)
#define I2C_SDA_Low  GPIO_WriteBit(SDA_PORT,SDA_PINS,RESET)
#define I2C_SCL_High GPIO_WriteBit(SCL_PORT,SCL_PINS,SET)
#define I2C_SCL_Low  GPIO_WriteBit(SCL_PORT,SCL_PINS,RESET)

#define get_SDA()   ( 0 == GPIO_ReadInputDataBit( SDA_PORT, SDA_PINS   )) ? 0 : 1 // ∂¡»° ‰»Î◊¥Ã¨
#define get_SCL()   ( 0 == GPIO_ReadInputDataBit( SCL_PORT, SCL_PINS   )) ? 0 : 1 // ∂¡»° ‰»Î◊¥Ã¨

//-----------------------------------------------------------------------------
void I2C_Config(void);
etError I2C_WriteNbyte(uint8_t Device,uint8_t bits,uint8_t addr,uint8_t *p,uint8_t number);
etError I2C_ReadNbyte(uint8_t Device,uint8_t bits,uint8_t addr,uint8_t *p,uint8_t number);


#endif