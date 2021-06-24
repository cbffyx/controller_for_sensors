#include "my_type.h"


//-----------------------------------------------------------------------------
void DS18B20_Reset()
{ 
	uint8_t ErrorTime=10;
    uint8_t CY = 1;
    
    while (CY)
    {
        DS18B20_DQ_OUT;             //�ı�DQ���ŷ�����Ϊ�����ʽ
        DS18B20_DQ_Low;             //�ͳ��͵�ƽ��λ�ź�
        delay_us(240);              //��ʱ����480us
		delay_us(240);
        DS18B20_DQ_High;            //�ͷ�������
        delay_us(60);               //�ȴ�60us
        DS18B20_DQ_IN;              //�ı�DQ���ŷ�����Ϊ���뷽ʽ
        CY = GPIO_ReadInputDataBit(DQ_PORT,DQ_PINS);   
		ErrorTime--;
		if(ErrorTime == 0) break;
        delay_us(240);              //�ȴ��豸�ͷ�������
		delay_us(180);
    }
}

//-----------------------------------------------------------------------------
uint8_t DS18B20_ReadByte()
{
    uint8_t i;
    uint8_t dat = 0;
    for (i=0; i<8; i++)             //8λ������
    {
        dat >>= 1;
        DS18B20_DQ_OUT;             //�ı�DQ���ŷ�����Ϊ�����ʽ
        DS18B20_DQ_Low;             //DQ����������
        delay_us(2);                //��ʱ�ȴ�
        DS18B20_DQ_High;            //׼������
        delay_us(2);                //������ʱ
        DS18B20_DQ_IN;              //�ı�DQ���ŷ�����Ϊ���뷽ʽ
        if(1 == get_DQ()) dat |= 0x80; 
        delay_us(60);               //�ȴ�ʱ��Ƭ����
    }
    return dat;
}

//-----------------------------------------------------------------------------
void DS18B20_WriteByte(uchar dat)
{
    uint8_t i;
    DS18B20_DQ_OUT;                 //�ı�DQ���ŷ�����Ϊ�����ʽ
    for (i=0; i<8; i++)             //8λ������
    {
        DS18B20_DQ_Low;             //DQ����������
        delay_us(2);                //��ʱ�ȴ�

        if((dat&0x01)==1) DS18B20_DQ_High;
        else              DS18B20_DQ_Low;

        delay_us(60);               //�ȴ�ʱ��Ƭ����
        DS18B20_DQ_High;            //�ָ�������
        dat >>= 1;                  //�ͳ�����
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
    
    DS18B20_DQ_IN;                  //�ı�DQ���ŷ�����Ϊ���뷽ʽ
    delay_ms(2);  
    while(0 == get_DQ())
    {
        delay_ms(1);      
        if(--TH == 0) return 65535;    
    }

	DS18B20_Reset();
	DS18B20_WriteByte(0xcc);
	DS18B20_WriteByte(0xbe);

	TL = DS18B20_ReadByte();        //���¶ȵ��ֽ�
    TH = DS18B20_ReadByte();        //���¶ȸ��ֽ�
	
	((uchar *)&temp)[1] = TL;
	((uchar *)&temp)[0] = TH;
    
    if(temp == 0xffff) return 65535;
	if(temp&0x8000)					//���¶� 
	{  
		temp = (~temp) + 1;
        return ((float)temp*0.0625*(-1)*0.9);
	}
    else
    {
        return ((float)temp*0.0625*0.9);
    }
}