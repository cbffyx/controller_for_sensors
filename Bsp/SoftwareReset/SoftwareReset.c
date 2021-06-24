#include "SoftwareReset.h"

void Software_Reset(void)
{
	
   __set_FAULTMASK(1); //关闭所有中断
   NVIC_SystemReset(); //软件复位会有延迟，中断仍会响应，所以在复位之前关闭中断
		
}

