#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP_RUNNER(INPData)
{
	RUN_TEST_CASE(INPData, adcDataProcessedNotProcessureAgain);
	RUN_TEST_CASE(INPData, adcDataProcessedWhileNoErr);
	RUN_TEST_CASE(INPData, adcDataProcessedWhile1ErrThenHave1ErrAtId0);
	RUN_TEST_CASE(INPData, adcDataProcessedWhile1ErrThenHave1ErrAtId1);
	RUN_TEST_CASE(INPData, adcDataProcessedWhile2ErrThenHave1ErrAtId1);

	RUN_TEST_CASE(INPData, cd4051DataProcessedWhileNoErr);
}