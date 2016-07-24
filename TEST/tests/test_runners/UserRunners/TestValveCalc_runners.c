#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP_RUNNER(ValveCalc)
{
	RUN_TEST_CASE(ValveCalc, PopAndPushSig);
	RUN_TEST_CASE(ValveCalc, CheckTotalStep);
	RUN_TEST_CASE(ValveCalc, valveCalcCheckProcess);
	RUN_TEST_CASE(ValveCalc, valveCalcValveMainWhen4TimesForward);
	RUN_TEST_CASE(ValveCalc, valveCalcValveMainWhen4TimesBack);
	RUN_TEST_CASE(ValveCalc, valveCalcValveMainWhenForwardAndBack);
}
