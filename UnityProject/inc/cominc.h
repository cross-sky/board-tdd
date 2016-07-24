#ifndef __COMMINCV2_H
#define __COMMINCV2_H
#include<stdio.h>
#include<stdlib.h>
#include <string.h>


//stm32����������,�Լ�������Ӧ��ͷ�ļ�
//Ϊ����vs�б��롢���ٴ���ָ���궨�壬���¶���
//....�Ѿ�������Ԥ��������
//#define __INLINE    __inline
//#define STM32F10X_MD 
//#define USE_STDPERIPH_DRIVER 
#ifdef __dsb_vs
void __dsb(uint8_t);
#endif // __dsb_vs


#include "stm32f10x_conf.h"
#include "stm32f10x.h"

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
#include "valve_calc.h"

#include "queue_process.h"
/*
#include <string.h>

#include "stm32f10x_spi.h"
#include "stm32f10x_it.h"
#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include "task.h"

#include "stdint.h"
#include "stm32_ub_led.h"
#include "stm32_ub_cd4051.h"
#include "stm32_ub_adc.h"
#include "event.h"
#include "stm32_ub_relay.h"
#include "input_process.h"
#include "queue_process.h"
#include "stm32_ub_dma_usart1.h"
#include "adc_filter.h"

#include "stm32_ub_valve.h"
#include "valve_calc.h"
#include "Tsystick.h"
*/




#endif

