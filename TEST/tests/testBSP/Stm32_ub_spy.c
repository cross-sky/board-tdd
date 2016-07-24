#include "Stm32_ub_adc_spy.h"

void vADCDmaChannel1_ISR(void)
{
	vADCsetSingleInputFlag(STATE_DATA_DONE);
}

void vUartDmaTxHandler_ISR(void)
{
	vUart_setTxStateOn();
}
void DMA_ClearITPendingBit(uint32_t DMAy_IT)
{

}

void DMA_Cmd(DMA_Channel_TypeDef* DMAy_Channelx, FunctionalState NewState)
{

}

void RCC_APB2PeriphClockCmd(uint32_t RCC_APB2Periph, FunctionalState NewState)
{

}

void GPIO_Init(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef* GPIO_InitStruct)
{

}

uint16_t CD4051_IOread(void)
{
	return 0x01;
}