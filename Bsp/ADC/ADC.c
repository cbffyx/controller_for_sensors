#include "adc.h"
#include "delay.h"

//#define ADC1_DR_Address                0x40012458//
__IO uint16_t RegularConvData_Tab[3];//�洢3����ѹֵ

void ADCNODMA_Config(void) 
{ 
	ADC_InitTypeDef ADC_InitStructure;
	RCC_HSICmd(ENABLE);
	while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);  
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	ADC_DeInit(ADC1);
	ADC_StructInit(&ADC_InitStructure);
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b; 
	ADC_InitStructure.ADC_ScanConvMode = DISABLE; //ɨ��ģʽ 
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;  //�Ƿ�����ת��
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None; //�ⲿ����ת��
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; //�Ҷ���
	ADC_InitStructure.ADC_NbrOfConversion = 1;
	ADC_Init(ADC1,&ADC_InitStructure);
	ADC_DelaySelectionConfig(ADC1, ADC_DelayLength_Freeze);//�Ƴٵ�ת��������ݱ���ȡ
	ADC_PowerDownCmd(ADC1, ADC_PowerDown_Idle_Delay, ENABLE);//���ӳٺͿ��н׶�ADC�ر�

	/* Enable ADC1 */
	ADC_Cmd(ADC1, ENABLE);
	/* Wait until the ADC1 is ready */
	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_ADONS) == RESET)
	{ 
	}
	
}

void  GetADCValue(uint32_t vbat[])
{
	uint8_t i, j;
	uint32_t RegularConvData_temp;

	for(i=0;i<3;i++)
    {
        switch(i)
        {
            case 0: 
                  ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 1, ADC_SampleTime_384Cycles); break;//M
            case 1:
                  ADC_RegularChannelConfig(ADC1, ADC_Channel_21, 1, ADC_SampleTime_384Cycles); break;//BAT
            case 2:
                  ADC_RegularChannelConfig(ADC1, ADC_Channel_17, 1, ADC_SampleTime_384Cycles); break;//Ref
        }
          /* Enable ADC1 */
          ADC_Cmd(ADC1, ENABLE);
					j=0;RegularConvData_temp=0;
					 while(j < 10)
					 {
								/* Start ADC1 Software Conversion */ 
								ADC_SoftwareStartConv(ADC1);
								while(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC)==RESET){
								}
				    	 RegularConvData_temp += ADC_GetConversionValue(ADC1);
						   j++;
							 delay_ms(10);
					 }
				vbat[i]= RegularConvData_temp / 10;
        ADC_ClearFlag(ADC1, ADC_FLAG_EOC);       
        ADC_Cmd(ADC1, DISABLE);
    }
}

