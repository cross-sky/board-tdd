#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP_RUNNER(RTdataStruct)
{
	RUN_TEST_CASE(RTdataStruct, add4RxReceiveOkThenAdd1RxToFail);
	RUN_TEST_CASE(RTdataStruct, TestFrameStartAtCommand2);
	RUN_TEST_CASE(RTdataStruct, TestFrameEndAtCommand3);
	RUN_TEST_CASE(RTdataStruct, TestCommand4);
}