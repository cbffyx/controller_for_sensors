#include "my_type.h"


UART_Define	Uart1={0,0,0,0,0,0,0,0,0};
//-----------------------------------------------------------------------------
void UART1_Config(void)
{
    USART_DeInit(USART1);
    GPIO_ExternalPullUpConfig(GPIOC,(GPIO_Pin_TypeDef)(GPIO_Pin_2 | GPIO_Pin_3),ENABLE);
    USART_Init(USART1,57600,USART_WordLength_8b,USART_StopBits_1,USART_Parity_No,(USART_Mode_TypeDef)(USART_Mode_Tx | USART_Mode_Rx));//设置USART参数9600，8N1，接收/发送
    USART_ITConfig (USART1,USART_IT_RXNE,ENABLE);//使能接收中断
    USART_Cmd (USART1,ENABLE);                   //使能USART  
}

void UART1_SendByte(USART_TypeDef* USARTx, uint8_t Byte)
{
    USART_SendData8(USARTx, Byte);              //发送数据 
    while(USART_GetFlagStatus (USARTx,USART_FLAG_TXE) == RESET);//等待发送完毕
}

void UART1_Prints(USART_TypeDef* USARTx, uint8_t *p, uint8_t len)
{
    uint8_t i=0; 
    for(i=0;i<len;i++)
    {
        UART1_SendByte(USARTx,*p);
        p++;
    }
    while(USART_GetFlagStatus (USARTx,USART_FLAG_TC) == RESET);//等待发送完毕
}

void UART1_SendString(USART_TypeDef* USARTx, char *str)
{
    uint8_t i=0; 
    do
    {
        UART1_SendByte(USARTx,*(str + i));
        i++;
    }while(*(str + i)!='\0');
    while(USART_GetFlagStatus (USARTx,USART_FLAG_TC) == RESET);//等待发送完毕
}

//-----------------------------------------------------------------------------
void DealUart1Tx(void)						     //串口1发送函数
{
	Uart1.TxLen -= 1;
	Uart1.Ptr = &Uart1.TxBuf[0];
	delay_us(10);
    USART_SendData8(USART1,*Uart1.Ptr++);
    USART_ITConfig (USART1,USART_IT_TC,ENABLE);  //使能发送完成中断
}

//////////////////////////printf//////////////////////////////
#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
    #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
    #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

PUTCHAR_PROTOTYPE                               //发送一个字符协议
{
    USART_SendData8(USART1,ch);                 //发送16进制
    while(!USART_GetFlagStatus (USART1,USART_FLAG_TXE));    //等待发送结束
    return (ch);
}