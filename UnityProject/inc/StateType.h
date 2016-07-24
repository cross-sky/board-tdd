#ifndef __STATE_H
#define __STATE_H
#include <stdint.h>

typedef enum StateType{
	STATE_OFF=0x00,
	STATE_ON,
	STATE_INIT,
	STATE_ERR,
	STATE_DATA_DONE,
	STATE_DATA_UNDONE
}StateType;


/************************************************************************/
/* 定时执行温度读取，高低压检测(连续以5ms间隔执行次数)，继电器输出，需要查询状态 

*/
/************************************************************************/
typedef struct StateStruct{
	uint16_t state;		//状态
	uint16_t runCount;		//已执行的次数-变量
	uint16_t needRunCount;		//每个周期需要连续执行次数，5ms/次间隔
	uint16_t cycleTime;		//间隔周期
	uint16_t dataProcessFlag;
}StateStruct,*ptrState;

typedef enum {FALSE = 0, TRUE = 1} bool;

bool State_checkStateIsOn(ptrState s);
void State_ChangeState(ptrState s,StateType state,uint16_t runCount, uint16_t needRunCount, uint16_t cycleTime);
StateType State_getState(ptrState s);
uint16_t State_getRunCount(ptrState s);
void State_setDataProcessedFlag(ptrState s, uint16_t flag);
uint16_t State_getDataProcessedFlag(ptrState s);
void State_addRunCount(ptrState s);
#endif

