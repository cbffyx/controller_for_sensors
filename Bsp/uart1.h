#ifndef _UART1_H_
#define _UART1_H_


/**************************************************************
*�궨��
***************************************************************/
#define 	OutTime0		5
#define 	RX_Lenth		20		
#define 	TX_Lenth		20

/**************************************************************
*�ṹ��
***************************************************************/
typedef struct
{
    uchar RxBuf[RX_Lenth]; 
	uchar TxBuf[TX_Lenth];	   
    uchar RxBusy;	 
    uchar TxLen;	 
    uchar RxLen;
    uchar Flag;			
    uchar OutTime;
	uchar *Ptr;  
	uchar Dec;  
}UART_Define;

/*******************************************************
****��������
*******************************************************/
extern UART_Define	Uart1;	//�����ⲿ����
extern UART_Define	ZM470S1;//�����ⲿ����

void UART1_Config(void);
void DealUart1Tx();		    //���ͺ���
void UART1_SendByte(USART_TypeDef* USARTx, uint8_t Byte);
void UART1_Prints(USART_TypeDef* USARTx, uint8_t *p, uint8_t len);
void UART1_SendString(USART_TypeDef* USARTx, char *str);


#endif