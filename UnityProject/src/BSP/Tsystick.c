#include "cominc.h"

#define D_5MS	200
#define D_1MS	1000			//1ms中断1次


void SystickInit( void )
{
	if ( SysTick_Config( SystemCoreClock / D_5MS ) )
	{
		while ( 1 );

	}			//配置不成功，则进入死循环;

}

void delay_us(uint32_t nus)
{
	uint32_t t0 = micros();
	while(micros() - t0 < nus);
}

void delay_ms(uint32_t nms)
{
	uint32_t t0 = micros();
	while (micros() - t0 < nms * 5000);
}


