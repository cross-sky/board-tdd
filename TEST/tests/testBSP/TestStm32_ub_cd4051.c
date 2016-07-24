#include "cominc.h"
#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP(BSP_cd4051);

ptrState externState;

uint16_t state;
uint16_t runCount;
uint16_t needRunCount;
uint16_t cycleTime;

ptrInputProcessStruct dataInp;
TEST_SETUP(BSP_cd4051)
{
	externState = BspAdc_getPtrStateCd4051();
	dataInp = INP_getDataInput();
}

TEST_TEAR_DOWN(BSP_cd4051)
{

}

TEST(BSP_cd4051, OffStateAllDoneChangeToOnstate)
{
	state = STATE_OFF;
	runCount = 8;
	needRunCount = 8;
	cycleTime = 10;

	State_setDataProcessedFlag(externState,STATE_DATA_DONE);

	State_ChangeState(externState, state,runCount,needRunCount,cycleTime);
	vCd4051Simpling();

	TEST_ASSERT_EQUAL(STATE_OFF, State_getState(externState));
	TEST_ASSERT_EQUAL(9, State_getRunCount(externState));

	vCd4051Simpling();
	TEST_ASSERT_EQUAL(STATE_ON, State_getState(externState));
	TEST_ASSERT_EQUAL(1, State_getRunCount(externState));
}

TEST(BSP_cd4051, simpling40Times)
{
	uint8_t i;
	state = STATE_OFF;
	runCount = 0;
	needRunCount = CDinMax*ADC_NeedRunCount;
	cycleTime = 100;

	State_ChangeState(externState, state,runCount,needRunCount,cycleTime);
	
	for(i=0;i<ADC_NeedRunCount;i++)
	{
		dataInp->dataCd4051[i]=0;
	}

	for (i=0;i<needRunCount;i++)
	{
		CD4051_startSimple(externState);
		State_addRunCount(externState);
	}

	for (i=0;i<ADC_NeedRunCount;i++)
	{
		TEST_ASSERT_EQUAL(0xff, dataInp->dataCd4051[i]);
	}
}
