#ifndef STM32_UB_ADC_SPY_H
#define STM32_UB_ADC_SPY_H
#include "cominc.h"

void GpioPort_output(GPIO_TypeDef* gpioPort, uint16_t pins, uint16_t dataOut);
void adcStartSimpling(void);
void vADCDmaChannel1_ISR(void);
void vUartDmaTxHandler_ISR(void);
uint16_t CD4051_IOread(void);
void vuartDmaTxDataEnable(uint16_t len, uint8_t *address);


#endif

