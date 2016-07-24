#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP_RUNNER(QueueProcess)
{
	RUN_TEST_CASE(QueueProcess, TestDelay3Min);
	RUN_TEST_CASE(QueueProcess, OffStateToOnstateThenToOffState);

}