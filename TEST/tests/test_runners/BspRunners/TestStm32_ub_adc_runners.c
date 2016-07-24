#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP_RUNNER(ADC_simpling)
{
	RUN_TEST_CASE(ADC_simpling, OffStateAllDoneChangeToOnstate);
	RUN_TEST_CASE(ADC_simpling, OffStateWhileDataUnprocessedRunCountWillNOtAdd);
	RUN_TEST_CASE(ADC_simpling, OffStateWhileInputUndoneThenRunCountWillNOtAdd);
	RUN_TEST_CASE(ADC_simpling, onStateChangeToOffState)
}
