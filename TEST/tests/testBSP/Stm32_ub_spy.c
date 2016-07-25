#include "Stm32_ub_adc_spy.h"

void vADCDmaChannel1_ISR(void)
{
	vADCsetSingleInputFlag(STATE_DATA_DONE);
}

void vUartDmaTxHandler_ISR(void)
{
	vUart_setTxStateOn();
}

void adcStartSimpling(void)
{

}

void GpioPort_output(GPIO_TypeDef* gpioPort, uint16_t pins, uint16_t dataOut)
{

}

uint16_t CD4051_IOread(void)
{
	return 0x01;
}

void vuartDmaTxDataEnable(uint16_t len, uint8_t *address)
{

}

void vUart2DmaTxHandler_ISR(void)
{
	vUart2_setTxStateOn();
}

