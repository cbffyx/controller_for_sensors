#include "SoftwareReset.h"

void Software_Reset(void)
{
	
   __set_FAULTMASK(1); //�ر������ж�
   NVIC_SystemReset(); //�����λ�����ӳ٣��ж��Ի���Ӧ�������ڸ�λ֮ǰ�ر��ж�
		
}

