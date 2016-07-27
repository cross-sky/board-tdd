#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP_RUNNER(Uart2Dma)
{
	RUN_TEST_CASE(Uart2Dma, RxCommand4RunTxfun1);
	RUN_TEST_CASE(Uart2Dma, RxAnUnvalidCommandThenNotProcessed)
}