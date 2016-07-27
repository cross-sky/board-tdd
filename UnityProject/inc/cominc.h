#ifndef __COMMINCV2_H
#define __COMMINCV2_H
#include<stdio.h>
#include<stdlib.h>
#include <string.h>


//stm32库内联函数,以及引用相应的头文件
//为了在vs中编译、减少错误，指定宏定义，以下都是
//....已经设置在预处理器中
//#define __INLINE    __inline
//#define STM32F10X_MD 
//#define USE_STDPERIPH_DRIVER 
#ifdef __dsb_vs
void __dsb(uint8_t);
#endif // __dsb_vs

#include <string.h>
#include<stdio.h>

#include "stm32f10x_conf.h"
#include "stm32f10x.h"
#include "stm32f10x_it.h"

#include "Tsystick.h"
#include "task.h"

#include "stm32_ub_led.h"
#include "StateType.h"
#include "stm32_ub_adc.h"
#include "ErrType.h"
#include "input_process.h"
#include "adc_filter.h"
#include "stm32_ub_cd4051.h"
#include "IoDetectErr.h"
#include "RTdataStruct.h"

#include "stm32_ub_relay.h"
#include "stm32_ub_dma_usart1.h"
#include "stm32_dma_uart2.h"
#include "valve_calc.h"

#include "queue_process.h"
/*
#include <string.h>


#include<stdlib.h>


#include "stdint.h"


*/




#endif

