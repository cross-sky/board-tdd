#include "cominc.h"
#include "unity.h"
#include "unity_fixture.h"
#include "Stm32_ub_adc_spy.h"

TEST_GROUP(ADC_simpling);

ptrState externState;

uint16_t state;
uint16_t runCount;
uint16_t needRunCount;
uint16_t cycleTime;
TEST_SETUP(ADC_simpling)
{
	externState = BspAdc_getPtrStateAdc();
}

TEST_TEAR_DOWN(ADC_simpling)
{

}


TEST(ADC_simpling, OffStateAllDoneChangeToOnstate)
{
	state = STATE_OFF;
	runCount = 3;
	needRunCount = 3;
	cycleTime = 5;

	State_setDataProcessedFlag(externState,STATE_DATA_DONE);
	vADCDmaChannel1_ISR();

	State_ChangeState(externState, state,runCount,needRunCount,cycleTime);
	vADCSimpling();	//没有执行这个.......
	TEST_ASSERT_EQUAL(STATE_OFF, State_getState(externState));
	TEST_ASSERT_EQUAL(4, State_getRunCount(externState));

//	vADCDmaChannel1_ISR();
	vADCSimpling();
	vADCDmaChannel1_ISR();
	vADCSimpling();
	vADCDmaChannel1_ISR();
	vADCSimpling();
	TEST_ASSERT_EQUAL(STATE_OFF, State_getState(externState));
	TEST_ASSERT_EQUAL(3, State_getRunCount(externState));
}

TEST(ADC_simpling, OffStateWhileDataUnprocessedRunCountWillNOtAdd)
{
	state = STATE_OFF;
	runCount = 4;
	needRunCount = 3;
	cycleTime = 5;

	State_setDataProcessedFlag(externState,STATE_DATA_UNDONE);
	vADCsetSingleInputFlag(STATE_DATA_DONE);

	State_ChangeState(externState, state,runCount,needRunCount,cycleTime);
	vADCSimpling();
	TEST_ASSERT_EQUAL(runCount, State_getRunCount(externState));
}

TEST(ADC_simpling, OffStateWhileInputUndoneThenRunCountWillNOtAdd)
{
	state = STATE_OFF;
	runCount = 4;
	needRunCount = 3;
	cycleTime = 5;

	State_setDataProcessedFlag(externState,STATE_DATA_DONE);
	vADCsetSingleInputFlag(STATE_DATA_UNDONE);

	State_ChangeState(externState, state,runCount,needRunCount,cycleTime);
	vADCSimpling();
	TEST_ASSERT_EQUAL(runCount, State_getRunCount(externState));
}

TEST(ADC_simpling, onStateChangeToOffState)
{
	state = STATE_ON;
	runCount = 2;
	needRunCount = 3;
	cycleTime = 5;

	State_setDataProcessedFlag(externState,STATE_DATA_DONE);
	vADCsetSingleInputFlag(STATE_DATA_DONE);

	State_ChangeState(externState, state,runCount,needRunCount,cycleTime);
	vADCSimpling();
	TEST_ASSERT_EQUAL(STATE_OFF, State_getState(externState));
}