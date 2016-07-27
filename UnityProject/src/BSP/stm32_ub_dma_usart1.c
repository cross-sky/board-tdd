#include "cominc.h"

 UartProcess_t uartRxProcess={
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

uint8_t TxBuff[TXBUF_SIZE]={0xfe,0xef};

uint8_t* vUart_getTxBuff(void)
{
	return TxBuff;
}

void vUart_setTxStateOn(void)
{
	stateUart = STATE_ON;
}


void vuartDmaTxDataEnable(uint16_t len, uint8_t *address)
{
	DMA1_Ch_Usart1_Tx->CNDTR =len;	//
	DMA1_Ch_Usart1_Tx->CMAR =(uint32_t)&TxBuff;
	DMA_Cmd(DMA1_Ch_Usart1_Tx,ENABLE);
}

void vUartDataReturn(uint8_t funcode)
{
	uint8_t len=0;
	switch(funcode)
	{
	case FunCode1:
		{
			//返回开关机 除霜 电加热等动作接收命令
			len = RT_command12SendReturn(FunCode1, TxBuff);
			break;
		}
	case FunCode2:
		{
			//返回发送参数设置接收命令
			len = RT_command12SendReturn(FunCode2, TxBuff);
			break;
		}
	case FunCode3:
		{
			len = RT_command3SendReturn(FunCode3,TxBuff);
			//return params data 
			break;
		}
	default:{
		break;
			}
	}
	if (len !=0)
	{
		vuartDmaTxDataEnable(len,TxBuff);
		stateUart = STATE_OFF;
	}
}

void vuartRxMessageAndReturnOk(uint8_t *buf)
{
	uint8_t i;
	i=*(buf+1);
	vUartDataReturn(i);
	//@@@@@@@@接收到的数据进行赋值
	switch(i)
	{
	case FunCode1:
		{
			//开关机
			RT_command1Receiverequest((Command1RequestDataStruct *)buf);
			break;
		}
	case FunCode2:
		{
			//设置参数
			RT_command2ReceiveRequest((Command2RequestDataStruct *)buf);
			break;
		}
	case FunCode3:
		{
			//仅查询，无参数设置
			break;
		}
	default:break;
	}
}

void vUartRxPopProcess(ptrUartNodesProcess nodes)
{
	RTdataNode dataNode;

	//1.在开始发送时置off，发送完中断中置on
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
			RT_addOutIndex(nodes);
			return;
		}
		//2. check add 
		if (RT_checkFrameEnd(&dataNode) == STATE_OFF)
		{
			RT_addOutIndex(nodes);
			return;
		}
		vuartRxMessageAndReturnOk(dataNode.buff);

		RT_addOutIndex(nodes);
	}
}




void UART1_Init(void)
{
	GPIO_InitTypeDef	GPIO_InitStructure;
	DMA_InitTypeDef		DMA_InitStructure;
	NVIC_InitTypeDef	NVIC_InitStructure;
	USART_InitTypeDef	USART_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|USART1_Tx_GPIO_CLK,ENABLE);
	RCC_APB2PeriphClockCmd(USART1_Tx_CLK,ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);

	// Configure USART_Rx as input floating 
	GPIO_InitStructure.GPIO_Pin = USART1_RxPin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(USART1_Rx_GPIO,&GPIO_InitStructure);

	// Configure USART_Tx as alternate function push-pull 
	GPIO_InitStructure.GPIO_Pin = USART1_TxPin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(USART1_Tx_GPIO,&GPIO_InitStructure);

	DMA_DeInit(DMA1_Ch_Usart1_Tx);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)USART1_Tx_DR_Base;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&TxBuff;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_BufferSize = TXBUF_SIZE;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

	DMA_Init(DMA1_Ch_Usart1_Tx,&DMA_InitStructure);
	DMA_ITConfig(DMA1_Ch_Usart1_Tx,DMA_IT_TC,ENABLE);
	DMA_ITConfig(DMA1_Ch_Usart1_Tx,DMA_IT_TE,ENABLE);
	USART_DMACmd(USART1_Tx, USART_DMAReq_Tx, ENABLE);
	DMA_Cmd(DMA1_Ch_Usart1_Tx,DISABLE);

	DMA_DeInit(DMA1_Ch_Usart1_Rx);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)USART1_Rx_DR_Base;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)puartGetRTxAddress(&uartRxProcess);
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = RXBUF_SIZE;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;

	DMA_Init(DMA1_Ch_Usart1_Rx,&DMA_InitStructure);
	DMA_ITConfig(DMA1_Ch_Usart1_Rx,DMA_IT_TC,ENABLE);//transmit complete
	DMA_ITConfig(DMA1_Ch_Usart1_Rx,DMA_IT_TE,ENABLE);//transmit enable
	USART_DMACmd(USART1_Rx, USART_DMAReq_Rx, ENABLE);
	DMA_Cmd(DMA1_Ch_Usart1_Rx,ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//Enable DMA Channel4 Interrupt 
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Ch_Usart1_Tx_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//Enable DMA Channel5 Interrupt 
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Ch_Usart1_Rx_IRQn;
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
	USART_Init(USART1_Tx, &USART_InitStructure);

	//空闲中断
	USART_ITConfig(USART1_Tx, USART_IT_IDLE , ENABLE);//
	USART_Cmd(USART1_Tx, ENABLE);
	USART_ClearFlag(USART1_Tx, USART_FLAG_TC);     //* 清发送外城标志，
}

void Usart1IdlHandle_ISR(void)
{
	uint16_t DataLen;
	if (USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)
	{
		DMA_Cmd(DMA1_Ch_Usart1_Rx, DISABLE);
		DataLen = RXBUF_SIZE - DMA_GetCurrDataCounter(DMA1_Ch_Usart1_Rx);
		//
		if (DataLen >0)
		{
			RT_uartRxPush((uint8_t)DataLen, &uartRxProcess);
		}
		DMA_ClearFlag(DMA1_FLAG_GL5|DMA1_FLAG_TC5|DMA1_FLAG_TE5|DMA1_FLAG_HT5);//清标志
		DMA1_Ch_Usart1_Rx->CNDTR =RXBUF_SIZE;	//
		DMA1_Ch_Usart1_Rx->CMAR =(uint32_t)puartGetRTxAddress(&uartRxProcess);
		DMA_Cmd(DMA1_Ch_Usart1_Rx, ENABLE);//处理完,重开DMA
		//读SR后读DR清除Idle
		DataLen = USART1->SR;
		DataLen = USART1->DR;
	}
	if (USART_GetITStatus(USART1, USART_IT_PE | USART_IT_FE | USART_IT_NE) != RESET)
	{
		USART_ClearITPendingBit(USART1, USART_IT_PE | USART_IT_FE | USART_IT_NE);
	}
	USART_ClearITPendingBit(USART1, USART_IT_TC);
	USART_ClearITPendingBit(USART1, USART_IT_IDLE);
}

void vUartDmaTxHandler_ISR(void)
{
	DMA_ClearITPendingBit(DMA1_IT_TC4|DMA1_IT_TE4);
	DMA_Cmd(DMA1_Ch_Usart1_Tx, DISABLE);
	vUart_setTxStateOn();
}

void vUartDmaRxHandle_ISR(void)
{
	DMA_ClearITPendingBit(DMA1_IT_TC5|DMA1_IT_TE5);
	DMA_Cmd(DMA1_Ch_Usart1_Rx, DISABLE);//关闭DMA,防止处理其间有数据, rx
	DMA1_Ch_Usart1_Rx->CNDTR =RXBUF_SIZE;	//
	DMA1_Ch_Usart1_Rx->CMAR =(uint32_t)puartGetRTxAddress(&uartRxProcess);
	DMA_Cmd(DMA1_Ch_Usart1_Rx, ENABLE);
}

void TaskUart1PopProcess(void)
{
	vUartRxPopProcess(&uartRxProcess);
}

