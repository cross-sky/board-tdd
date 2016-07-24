#ifndef STM32_UB_ADC_SPY_H
#define STM32_UB_ADC_SPY_H
#include "cominc.h"
//#include "stm32_ub_adc.h"
//#include "stm32_ub_dma_usart1.h"

void vADCDmaChannel1_ISR(void);
void vUartDmaTxHandler_ISR(void);
void DMA_ClearITPendingBit(uint32_t DMAy_IT);
void DMA_Cmd(DMA_Channel_TypeDef* DMAy_Channelx, FunctionalState NewState);
void RCC_APB2PeriphClockCmd(uint32_t RCC_APB2Periph, FunctionalState NewState);
void GPIO_Init(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef* GPIO_InitStruct);
uint16_t CD4051_IOread(void);
#endif

