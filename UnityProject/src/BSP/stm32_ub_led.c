#include "cominc.h"


LED_t LED[]={
	{LED1 ,GPIOB,GPIO_Pin_5,RCC_APB2Periph_GPIOB,LED_OFF},   // 
};

void UB_LED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	uint8_t led_name;
	
	for (led_name=0; led_name < LED_ANZ; led_name++)
	{
		RCC_APB2PeriphClockCmd(LED[led_name].LED_CLK , ENABLE); 

		GPIO_InitStructure.GPIO_Pin = LED[led_name].LED_PIN;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
		GPIO_Init(LED[led_name].LED_PORT, &GPIO_InitStructure);

		// Default Wert einstellen
		if(LED[led_name].LED_INIT==LED_ON) {
			UB_Led_Off((LED_NAME_t)led_name);
		}
		else {
			UB_Led_On((LED_NAME_t)led_name);
		}
	}
}

void UB_Led_Off(LED_NAME_t led_name)
{
	LED[led_name].LED_PORT->BRR = LED[led_name].LED_PIN;
}

void UB_Led_On(LED_NAME_t led_name)
{
	LED[led_name].LED_PORT->BSRR = LED[led_name].LED_PIN;
} 

void UB_Led_Toggle(LED_NAME_t led_name)
{
	LED[led_name].LED_PORT->ODR ^= LED[led_name].LED_PIN;
}

void UB_Led_Switch(LED_NAME_t led_name, LED_STATUS_t status)
{
	if(status==LED_OFF) {
		UB_Led_Off(led_name);
	}
	else {
		UB_Led_On(led_name);
	}
}

void LED1on(void)
{
	UB_Led_Toggle(LED1);
}


