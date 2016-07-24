#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP_RUNNER(Uart1Dma)
{
	RUN_TEST_CASE(Uart1Dma, RxCommand1ReturnOK);
	RUN_TEST_CASE(Uart1Dma, RxCommand2ReturnOK);
	RUN_TEST_CASE(Uart1Dma, RxCommand3ReturnOK);
}