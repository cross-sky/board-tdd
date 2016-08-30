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
	RUN_TEST_CASE(ValveCalc, valveCalcValveMainWhenBackAndForward);
	RUN_TEST_CASE(ValveCalc, TestValveCalc_calcValveSub);
	RUN_TEST_CASE(ValveCalc, TestValveCalc_HotwaterModelChangeToColdModel);
	RUN_TEST_CASE(ValveCalc, ColdModelValveCalcCheckProcess);
	RUN_TEST_CASE(ValveCalc, ColdModelValveCalcValveMainWhen4TimesBack4TimsFward);
}
