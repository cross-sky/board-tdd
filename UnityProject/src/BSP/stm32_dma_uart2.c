#include "cominc.h"

#define RTx2BUF_MAX 3
static UartProcess_t uart2RxProcess={
	0,0,RTxBUF_MAX_COUNTER,
	{
		{0,{0}},
		{0,{0}},
		{0,{0}},
		{0,{0}},
		{0,{0}}
	}
};

static uint8_t stateUart = STATE_ON;

#define  Tx2BUF_MAX 200
static uint8_t TxBuff[Tx2BUF_MAX]={0};

TabUart2TxFun uart2TxFun={
	0,
	{vUart2TxFun0,vUart2TxFun1,vUart2TxFun0}
};

TabUart2TxFun* Uart2_getTxFunTab(void)
{
	return &uart2TxFun;
}

UartProcess_t* Uart2_getRxTab(void)
{
	return &uart2RxProcess;
}

uint8_t* vUart2_getTxBuff(void)
{
	return TxBuff;
}

void vUart2_setTxStateOn(void)
{
	stateUart = STATE_ON;
}

void vuart2DmaTxDataEnable(uint16_t len, uint8_t *address)
{
	DMA1_Ch_Usart2_Tx->CNDTR =len;	//
	DMA1_Ch_Usart2_Tx->CMAR =(uint32_t)&TxBuff;
	DMA_Cmd(DMA1_Ch_Usart2_Tx,ENABLE);
}

void vUart2DmaTxHandler_ISR(void)
{
	DMA_ClearITPendingBit(DMA1_IT_TC7|DMA1_IT_TE7);
	DMA_Cmd(DMA1_Ch_Usart2_Tx, DISABLE);
	vUart2_setTxStateOn();
}

void UART2_Init(void)
{
	GPIO_InitTypeDef	GPIO_InitStructure;
	DMA_InitTypeDef		DMA_InitStructure;
	NVIC_InitTypeDef	NVIC_InitStructure;
	USART_InitTypeDef	USART_InitStructure;

	//������uart1��ʼ����ǰ��Ȼ����ܳ�ʼ��uart2
	//RCC_APB2PeriphClockCmd(USART2_Tx_GPIO_CLK,ENABLE);
	RCC_APB1PeriphClockCmd(USART2_Tx_CLK,ENABLE);
	//RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);

	// Configure USART_Rx as input floating 
	GPIO_InitStructure.GPIO_Pin = USART2_RxPin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(USART2_Rx_GPIO,&GPIO_InitStructure);

	// Configure USART_Tx as alternate function push-pull 
	GPIO_InitStructure.GPIO_Pin = USART2_TxPin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(USART2_Tx_GPIO,&GPIO_InitStructure);

	DMA_DeInit(DMA1_Ch_Usart2_Tx);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)USART2_Tx_DR_Base;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&TxBuff;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_BufferSize = Tx2BUF_MAX;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

	DMA_Init(DMA1_Ch_Usart2_Tx,&DMA_InitStructure);
	DMA_ITConfig(DMA1_Ch_Usart2_Tx,DMA_IT_TC,ENABLE);
	DMA_ITConfig(DMA1_Ch_Usart2_Tx,DMA_IT_TE,ENABLE);
	USART_DMACmd(USART2_Tx, USART_DMAReq_Tx, ENABLE);
	DMA_Cmd(DMA1_Ch_Usart2_Tx,DISABLE);

	DMA_DeInit(DMA1_Ch_Usart2_Rx);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)USART2_Rx_DR_Base;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)puartGetRTxAddress(&uart2RxProcess);
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = RXBUF_SIZE;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;

	DMA_Init(DMA1_Ch_Usart2_Rx,&DMA_InitStructure);
	DMA_ITConfig(DMA1_Ch_Usart2_Rx,DMA_IT_TC,ENABLE);//transmit complete
	DMA_ITConfig(DMA1_Ch_Usart2_Rx,DMA_IT_TE,ENABLE);//transmit enable
	USART_DMACmd(USART2_Rx, USART_DMAReq_Rx, ENABLE);
	DMA_Cmd(DMA1_Ch_Usart2_Rx,ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//Enable DMA Channel4 Interrupt 
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Ch_Usart2_Tx_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//Enable DMA Channel5 Interrupt 
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Ch_Usart2_Rx_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 6;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	USART_InitStructure.USART_BaudRate = Bounds;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2_Tx, &USART_InitStructure);

	//�����ж�
	USART_ITConfig(USART2_Tx, USART_IT_IDLE , ENABLE);//
	USART_Cmd(USART2_Tx, ENABLE);
	USART_ClearFlag(USART2_Tx, USART_FLAG_TC);     //* �巢����Ǳ�־��
}

void Usart2IdlHandle_ISR(void)
{
	uint16_t DataLen;
	if (USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)
	{
		DMA_Cmd(DMA1_Ch_Usart2_Rx, DISABLE);
		DataLen = RXBUF_SIZE - DMA_GetCurrDataCounter(DMA1_Ch_Usart2_Rx);
		//
		if (DataLen >0)
		{
			RT_uartRxPush((uint8_t)DataLen, &uart2RxProcess);
		}
		DMA_ClearFlag(DMA1_FLAG_GL6|DMA1_FLAG_TC6|DMA1_FLAG_TE6|DMA1_FLAG_HT6);//���־
		DMA1_Ch_Usart2_Rx->CNDTR =RXBUF_SIZE;	//
		DMA1_Ch_Usart2_Rx->CMAR =(uint32_t)puartGetRTxAddress(&uart2RxProcess);
		DMA_Cmd(DMA1_Ch_Usart2_Rx, ENABLE);//������,�ؿ�DMA
		//��SR���DR���Idle
		DataLen = USART2->SR;
		DataLen = USART2->DR;
	}
	if (USART_GetITStatus(USART2, USART_IT_PE | USART_IT_FE | USART_IT_NE) != RESET)
	{
		USART_ClearITPendingBit(USART2, USART_IT_PE | USART_IT_FE | USART_IT_NE);
	}
	USART_ClearITPendingBit(USART2, USART_IT_TC);
	USART_ClearITPendingBit(USART2, USART_IT_IDLE);
}

void vUart2DmaRxHandle_ISR(void)
{
	DMA_ClearITPendingBit(DMA1_IT_TC6|DMA1_IT_TE6);
	DMA_Cmd(DMA1_Ch_Usart2_Rx, DISABLE);//�ر�DMA,��ֹ�������������, rx
	DMA1_Ch_Usart2_Rx->CNDTR =RXBUF_SIZE;	//
	DMA1_Ch_Usart2_Rx->CMAR =(uint32_t)puartGetRTxAddress(&uart2RxProcess);
	DMA_Cmd(DMA1_Ch_Usart2_Rx, ENABLE);
}

uint8_t vuartRxMessage(uint8_t *buf)
{
	uint8_t i,j=0;
	i=*(buf+1);
	switch(i)
	{
	case FunCode4:
		{
			j=RT_command4ReceiveRequest((Command4RequestDataStruct *)buf);
			break;
		}
	default:
		break;
	}
	return j;
}

void vUart2RxPopProcess(ptrUartNodesProcess nodes)
{
	RTdataNode dataNode;

	//1.�ڿ�ʼ����ʱ��off���������ж�����on
	if (stateUart == STATE_OFF)
	{
		return;
	}

	if(nodes->out != nodes->in)
	{
		//.....
		dataNode.length = nodes->node[nodes->out].length;
		dataNode.buff = nodes->node[nodes->out].buff;
		//1.check frame start code
		if (RT_checkFrameStart(&dataNode) == STATE_OFF)
		{
			return;
		}
		//2. check add 
		if (RT_checkFrameEnd(&dataNode) == STATE_OFF)
		{
			return;
		}
		uart2TxFun.indesFun = vuartRxMessage(dataNode.buff);

		nodes->out++;
		if (nodes->out>=nodes->max)
		{
			nodes->out=0;
		}
	}
}

void vUart2TxFun1(void)
{
	RT_command4SendReturn(TxBuff);
}
void vUart2TxFun0(void)
{

}

//1s/excute times
void TaskUart2TxStrData(void)
{
	uart2TxFun.fun[uart2TxFun.indesFun]();
}