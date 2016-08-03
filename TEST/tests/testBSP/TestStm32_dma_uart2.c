#include "cominc.h"
#include "unity.h"
#include "unity_fixture.h"

//注意测试值与原值变量名不能一致，否则会覆盖
TabUart2TxFun* puart2TxFun;

#define NodeMax 5
static UartProcess_t uartRxProcess={
	0
};

static void setFrame(uint8_t i)
{
	uartRxProcess.node[i].length = 10;
	uartRxProcess.node[i].buff[0]=0xfe;
	uartRxProcess.node[i].buff[1]=0xfe;
	uartRxProcess.node[i].buff[2]=0xef;
}

TEST_GROUP(Uart2Dma);

TEST_SETUP(Uart2Dma)
{
	memset(&uartRxProcess,0,sizeof(UartProcess_t));
	uartRxProcess.max = NodeMax;
	puart2TxFun=Uart2_getTxFunTab();
}

TEST_TEAR_DOWN(Uart2Dma)
{

}

TEST(Uart2Dma, RxCommand4RunTxfun1)
{
	RTdataNode uartNode;
	uartNode.length = 10;
	uartNode.buff = &(uartRxProcess.node[0].buff[0]);
	setFrame(0);
	RT_command4CreateRequest((Command4RequestDataStruct *)&(uartRxProcess.node[0].buff[3]));
	TEST_ASSERT_EQUAL(STATE_ON, RT_uartRxPush(10, &uartRxProcess));
	vUart2RxPopProcess(&uartRxProcess);
	vUart2DmaTxHandler_ISR();

	TEST_ASSERT_EQUAL(1,puart2TxFun->indesFun);
}

TEST(Uart2Dma, RxAnUnvalidCommandThenNotProcessed)
{
	uint8_t i;
	RTdataNode uartNode;
	uartNode.length = 10;
	uartNode.buff = &(uartRxProcess.node[0].buff[0]);

	for (i=0; i<=uartNode.length; i++)
	{
		uartNode.buff[i]=0xee;
	}
	TEST_ASSERT_EQUAL(STATE_ON, RT_uartRxPush(10, &uartRxProcess));
	vUart2RxPopProcess(&uartRxProcess);
	vUart2DmaTxHandler_ISR();
	
	TEST_ASSERT_EQUAL(NodeMax,RT_getNodesCount(&uartRxProcess));
}

