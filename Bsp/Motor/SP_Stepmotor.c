/**
  * @brief  电机 program.
  * @description None
  * @param  teper
  */

#include "SP_Stepmotor.h"

#define ENA_HIGH()		GPIO_WriteBit(GPIOB, GPIO_Pin_5, Bit_SET)
#define ENA_LOW()		GPIO_WriteBit(GPIOB, GPIO_Pin_5, Bit_RESET)
#define INT1_HIGH()		GPIO_WriteBit(GPIOB, GPIO_Pin_4, Bit_SET)
#define INT1_LOW()		GPIO_WriteBit(GPIOB, GPIO_Pin_4, Bit_RESET)
#define INT2_HIGH()		GPIO_WriteBit(GPIOB, GPIO_Pin_3, Bit_SET)
#define INT2_LOW()		GPIO_WriteBit(GPIOB, GPIO_Pin_3, Bit_RESET)



void Motor_GPIO_Init(void)
{
	GPIO_InitTypeDef    GPIO_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB,&GPIO_InitStructure);

	ENA_LOW();	
}



/*
���ģ�
�ĸ�״̬�� 1�� AB ����ѹ�� 2�� A-B ����ѹ�� 3�� A-B-����ѹ�� 4�� AB-����ѹ��
	1 2 3 4
A 	1 0 0 1
A- 	0 1 1 0
B 	1 1 0 0
B- 	0 0 1 1

*/
void Motor_cw(uint32_t circle, uint16_t nms)//circle循环数delay转�?
{
	uint32_t i;
	Motor_GPIO_Init();
	ENA_HIGH();
	for(i=0;i<=circle;i++)
	{
		INT1_LOW();
		INT2_LOW();
		delay_ms(3);
		INT1_LOW();
		INT2_HIGH();
		delay_ms(3);
		INT1_HIGH();
		INT2_HIGH();
		delay_ms(3);
		INT1_HIGH();
		INT2_LOW();
		delay_ms(3);
		//delay_ms(nms);
	}
	ENA_LOW();
}

//��ת
void Motor_ccw(uint32_t circle,uint16_t nms)
{
	uint32_t i;
	Motor_GPIO_Init();
	ENA_HIGH();
	for(i=0;i<=circle;i++)
	{
		INT1_HIGH();
		INT2_LOW();
		delay_ms(3);
		INT1_HIGH();
		INT2_HIGH();
		delay_ms(3);
		INT1_LOW();
		INT2_HIGH();
		delay_ms(3);
		INT1_LOW();
		INT2_LOW();
		delay_ms(3);

		delay_ms(nms);
	}
	ENA_LOW();
}

