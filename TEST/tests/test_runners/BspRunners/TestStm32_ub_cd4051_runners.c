#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP_RUNNER(BSP_cd4051)
{
	RUN_TEST_CASE(BSP_cd4051, OffStateAllDoneChangeToOnstate);
	RUN_TEST_CASE(BSP_cd4051, simpling40Times);
}