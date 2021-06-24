#include "usart.h"	  
#include "stdio.h"
#include "string.h"
#include "TIM.h"


//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
uint16_t usart1RxCnt = 0;
char usart1RxBuffer[usart1RxBufferSize] = {0};     
//uint16_t usart2RxCnt = 0;
//char usart2RxBuffer[usart2RxBufferSize] = {0};     
uint16_t usart3RxCnt; 
char usart3RxBuffer[usart3RxBufferSize] = {0};     			

#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

PUTCHAR_PROTOTYPE //模组端
{
	while((USART3->SR&0X40)==0)
	{}//循环发送,直到发送完毕   
	USART3->DR = (u8) ch;      
	return ch;
}
//PUTCHAR_PROTOTYPE //电脑端
//{
//  	while((USART1->SR&0X40)==0)
//		{}//循环发送,直到发送完毕   
//    USART1->DR = (u8) ch;      
//	return ch;
//}
void UART1_send_byte(char data)
{
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	USART_SendData(USART1, data);
}
/*
void UART2_send_byte(char data)//BlueToothUse
{
	while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
	USART_SendData(USART2, data);
}
*/
void UART3_send_byte(char data)//LoraUse
{
	while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
	USART_SendData(USART3, data);
}

//初始化IO 串口1 
//bound:波特率
void uart1_init(u32 bound){
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_AHBPeriphClockCmd (RCC_AHBPeriph_GPIOA,ENABLE);//GPIOA
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);	//使能USART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);
	/*
	*  USART1_TX -> PA9 , USART1_RX -> PA10
	*/				
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	         
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; //
	GPIO_Init(GPIOA, &GPIO_InitStructure);		

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	         
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	//Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

	//USART 初始化设置
	USART_DeInit(USART1);
	USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

	USART_Init(USART1, &USART_InitStructure); //初始化串口
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启中断
	USART_Cmd(USART1, ENABLE);                    //使能串口 
	
//	usart1RxCnt=0;
	
  TIM6_UART_Init_ms(10);    
}

/*
void uart2_init(u32 bound)//BlueToothUse
{
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_AHBPeriphClockCmd (RCC_AHBPeriph_GPIOA,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2);
	//
	//  USART2_TX -> PA2 , USART2_RX -> PA3
	//				
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;	         
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; //no上拉
	GPIO_Init(GPIOA, &GPIO_InitStructure);		

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;	         
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//GPIO_PuPd_UP; //上拉
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	//Usart2 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

	//USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

	USART_Init(USART2, &USART_InitStructure); //初始化串口
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启中断
	USART_Cmd(USART2, ENABLE);                    //使能串口 
	
	//usart2RxCnt=0;
	
  TIM4_UART_Init_ms(10);    
}
*/

//初始化IO 串口3 
//bound:波特率
void uart3_init(u32 bound)//LoraUse
{
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_USART3);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_USART3);
	/*
	*  USART3_TX -> PB10 , USART3_RX -> PB11
	*/				
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	         
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; //
	GPIO_Init(GPIOB, &GPIO_InitStructure);		

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;	         
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; 
	GPIO_Init(GPIOB, &GPIO_InitStructure);	
	//Usart3 NVIC 配置

	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//子优先级1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

	//USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(USART3, &USART_InitStructure); //初始化串口
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启中断
	USART_Cmd(USART3, ENABLE);                    //使能串口 

	//usart3RxCnt=0;
	
  TIM3_UART_Init_ms(10);    
}

void Uart1_SendStr(char*SendBuf)//串口1打印数据代码
{	
	while(*SendBuf)
	{
	  while((USART1->SR&0X40)==0)
		{
		}//等待发送完成
		USART1->DR = (u8) *SendBuf; 
		SendBuf++;
	}
  while((USART1->SR&0X40)==0)
	{
	}//等待发送完成
}

/*
void Uart2_SendStr(char*SendBuf)//BlueToothUse//串口1打印数据代码
{	
	while(*SendBuf)
	{
	  while((USART2->SR&0X40)==0)
		{
		}//等待发送完成
		USART2->DR = (u8) *SendBuf; 
		SendBuf++;
	}
	  while((USART2->SR&0X40)==0)
		{
		}//等待发送完成
}
*/

void Uart3_SendStr(char*SendBuf)//LoraUse//串口1打印数据代码
{	
	while(*SendBuf)
	{
	  while((USART3->SR&0X40)==0)
		{
		}//等待发送完成
		USART3->DR = (u8) *SendBuf; 
		SendBuf++;
	}
	  while((USART3->SR&0X40)==0)
		{
		}//等待发送完成
}

void Uart1_SendData(char *data, uint16_t len)
{
	uint16_t i = 0;
	
	for(i=0; i<len; i++)
	{
		while((USART1->SR&0X40)==0)
		{
		}//等待发送完成
		USART1->DR = (u8) *data; 
		data++;
	}
		while((USART1->SR&0X40)==0)
		{
		}//等待发送完成
}

void Uart1_SendHEXData(char *data, uint16_t len)
{
	uint16_t i = 0;
	uint8_t tempusartdata;
	for(i=0; i<len; i++)
	{
		while((USART1->SR&0X40)==0)
		{
		}//等待发送完成
		tempusartdata=(uint8_t) *data;
		tempusartdata=tempusartdata>>4;
		if (tempusartdata<=9) 
		{
			tempusartdata=tempusartdata+'0';
		}
		else
		{
			tempusartdata=tempusartdata-10+'A';
		}
		USART1->DR = tempusartdata; 
		while((USART1->SR&0X40)==0)
		{
		}//等待发送完成
		tempusartdata=(uint8_t) *data;
		tempusartdata=tempusartdata&0x0f;
		if (tempusartdata<=9) 
		{
			tempusartdata=tempusartdata+'0';
		}
		else
		{
			tempusartdata=tempusartdata-10+'A';
		}
		USART1->DR = tempusartdata; 
		data++;
	}
		while((USART1->SR&0X40)==0)
		{
		}//等待发送完成
}

/*
void Uart2_SendData(char *data, uint16_t len)//BlueToothUse
{
	uint16_t i = 0;
	
	for(i=0; i<len; i++)
	{
		while((USART2->SR&0X40)==0)
		{
		}//等待发送完成
		USART2->DR = (u8) *data; 
		data++;
	}
		while((USART2->SR&0X40)==0)
		{
		}//等待发送完成
}
*/

void Uart3_SendData(char *data, uint16_t len)//LoraUse
{
	uint16_t i = 0;
	
	for(i=0; i<len; i++)
	{
		while((USART3->SR&0X40)==0)
		{
		}//等待发送完成
		USART3->DR = (u8) *data; 
		data++;
	}
		while((USART3->SR&0X40)==0)
		{
		}//等待发送完成
}

void USART1_IRQHandler(void)                	//串口1中断服务程序
{
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断，可以扩展来控制
	{
		if (usart1RxCnt<usart1RxBufferSize-2)
			usart1RxBuffer[usart1RxCnt++] =USART_ReceiveData(USART1);//接收模块的数据
		TIM_SetCounter(TIM6,0);
		TIM_Cmd(TIM6,ENABLE);
	} 
} 

/*
void USART2_IRQHandler(void)//BlueToothUse                	//串口2中断服务程序
{
	uint8_t tmpdata_IRQ_USART2;
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //接收模块返回的数据
	{
		tmpdata_IRQ_USART2 = USART_ReceiveData(USART2);//接收模块的数据;
		if ((usart2RxCnt==0) && (tmpdata_IRQ_USART2 == 0xff)) return;
		if (usart2RxCnt<usart2RxBufferSize-2)
		usart2RxBuffer[usart2RxCnt++] = tmpdata_IRQ_USART2;
		TIM_SetCounter(TIM4,0);
		TIM_Cmd(TIM4,ENABLE);
	} 
} 	
*/

void USART3_IRQHandler(void)//LoraUse                	//串口3中断服务程序
{
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //接收模块返回的数据
	{
		if (usart3RxCnt<usart3RxBufferSize-2)
		usart3RxBuffer[usart3RxCnt++]=USART_ReceiveData(USART3);//接收模块的数据;
		TIM_SetCounter(TIM3,0);
		TIM_Cmd(TIM3,ENABLE);
	} 
} 	

/*
void PrintUsart2Buffer(void)//BlueToothUse
{
	Uart1_SendStr(usart2RxBuffer);
}

void PrintClrUsart2Buffer(void)
{
	Uart1_SendStr(usart2RxBuffer);
	usart2RxCnt=0;
	memset(usart2RxBuffer, 0x00, usart2RxBufferSize);
}
*/
void PrintUsart3Buffer(void)//LoraUse
{
	Uart1_SendStr(usart3RxBuffer);
}

void PrintClrUsart3Buffer(void)
{
	Uart1_SendHEXData(usart3RxBuffer,usart3RxCnt);
	//Uart1_SendStr(usart3RxBuffer);
	usart3RxCnt=0;
	memset(usart3RxBuffer, 0x00, usart3RxBufferSize);
}
void ClrUsart3Buffer(void)
{
	usart3RxCnt=0;
	memset(usart3RxBuffer, 0x00, usart3RxBufferSize);
}

