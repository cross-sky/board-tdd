#ifndef  __STM32_DMA_UART2_h
#define __STM32_DMA_UART2_h

typedef void(*ptrUart2TxFun)(void);
#define  Uart2TxFunMax 3

typedef struct TabUart2TxFun{
	uint16_t indesFun;
	ptrUart2TxFun fun[Uart2TxFunMax];
}TabUart2TxFun;

#define USART2_Rx				USART2
#define USART2_Rx_GPIO			GPIOA
#define USART2_Rx_CLK			RCC_APB1Periph_USART2
#define USART2_Rx_GPIO_CLK		RCC_APB2Periph_GPIOA
#define USART2_RxPin			GPIO_Pin_3

#define USART2_Tx				USART2
#define USART2_Tx_GPIO			GPIOA
#define USART2_Tx_CLK			RCC_APB1Periph_USART2
#define USART2_Tx_GPIO_CLK		RCC_APB2Periph_GPIOA
#define USART2_TxPin			GPIO_Pin_2
//这是一个双向寄存器，包含了TDR和RDR，对它读操作，读取的是RDR寄存器的值，对它的写操作，实际上是写到TDR寄存器的
#define USART2_Tx_DR_Base		0x40004404
#define DMA1_Ch_Usart2_Tx		DMA1_Channel7
#define DMA1_Ch_Usart2_Tx_IRQn	DMA1_Channel7_IRQn

#define USART2_Rx_DR_Base		0x40004404
#define DMA1_Ch_Usart2_Rx		DMA1_Channel6
#define DMA1_Ch_Usart2_Rx_IRQn	DMA1_Channel6_IRQn

void vUart2TxFun0(void);
void vUart2TxFun1(void);
void vUart2RxPopProcess(ptrUartNodesProcess nodes);
void vUart2_setTxStateOn(void);

void UART2_Init(void);
void TaskUart2TxStrData(void);

TabUart2TxFun* Uart2_getTxFunTab(void);
UartProcess_t* Uart2_getRxTab(void);

void vUart2DmaRxHandle_ISR(void);
void vUart2DmaTxHandler_ISR(void);
void Usart2IdlHandle_ISR(void);


#endif

