#ifndef _STM32_UB_DMA_USART1_H
#define _STM32_UB_DMA_USART1_H

#define Bounds	9600

#define USART1_Rx				USART1
#define USART1_Rx_GPIO			GPIOA
#define USART1_Rx_CLK			RCC_APB2Periph_USART1
#define USART1_Rx_GPIO_CLK		RCC_APB2Periph_GPIOA
#define USART1_RxPin			GPIO_Pin_10

#define USART1_Tx				USART1
#define USART1_Tx_GPIO			GPIOA
#define USART1_Tx_CLK			RCC_APB2Periph_USART1
#define USART1_Tx_GPIO_CLK		RCC_APB2Periph_GPIOA
#define USART1_TxPin			GPIO_Pin_9
//这是一个双向寄存器，包含了TDR和RDR，对它读操作，读取的是RDR寄存器的值，对它的写操作，实际上是写到TDR寄存器的
#define USART1_Tx_DR_Base		0x40013804
#define DMA1_Ch_Usart1_Tx		DMA1_Channel4
#define DMA1_Ch_Usart1_Tx_IRQn	DMA1_Channel4_IRQn

#define USART1_Rx_DR_Base		0x40013804
#define DMA1_Ch_Usart1_Rx		DMA1_Channel5
#define DMA1_Ch_Usart1_Rx_IRQn	DMA1_Channel5_IRQn

void vUartDmaTxHandler_ISR(void);
void vUart_setTxStateOn(void);
uint8_t* vUart_getTxBuff(void);

void vUartRxPopProcess(ptrUartNodesProcess nodes);

/*







typedef enum{
	FrameStart1Bit=0x00,
	FrameStart2Bit,
	FrameDataLenBit,
	FrameFunCodeBit
}RTxEnum;



void vUartRxPopProcess(void);
void UART1_Init(void);
void Usart1IdlHandle_ISR(void);
void vUartDmaRxHandle_ISR(void);
void vUartDmaTxHandler_ISR(void);

*/

#endif


