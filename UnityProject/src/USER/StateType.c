#include "StateType.h"

StateType State_getState(ptrState s)
{
	return (StateType)s->state;
}

uint16_t State_getRunCount(ptrState s)
{
	return s->runCount;
}

uint16_t isNextSimpleCycle(ptrState s)
{
	return (++(s->runCount)) >= s->cycleTime;
}

bool State_checkStateIsOn(ptrState s)
{
	if (s->state == STATE_ON)
	{
		return TRUE;
	}
	else
	{
		//下一个检测时间到
		if (++(s->runCount) >= s->cycleTime)
		{
			//设置状态为开始
			s->state = STATE_ON;
			s->runCount = 0;
			return TRUE;
		}
		else
			return FALSE;
	}
}

void State_ChangeState(ptrState s,StateType state,uint16_t runCount, uint16_t needRunCount, uint16_t cycleTime)
{
	s->state = state;
	s->runCount = runCount;
	s->needRunCount = needRunCount;
	s->cycleTime = cycleTime;
}

uint16_t State_getDataProcessedFlag(ptrState s)
{
	return s->dataProcessFlag;
}

void State_setDataProcessedFlag(ptrState s, uint16_t flag)
{
	s->dataProcessFlag = flag;
}

void State_addRunCount(ptrState s)
{
	s->runCount++;
	if (s->runCount >= s->needRunCount)
	{
		s->state = STATE_OFF;
		s->dataProcessFlag = STATE_DATA_UNDONE;
	}
}

