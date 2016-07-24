#include "cominc.h"
#include "unity.h"
#include "unity_fixture.h"
#include <string.h>

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

TEST_GROUP(RTdataStruct);


TEST_SETUP(RTdataStruct)
{
	memset(&uartRxProcess,0,sizeof(UartProcess_t));
	uartRxProcess.max = 5;
}

TEST_TEAR_DOWN(RTdataStruct)
{

}


TEST(RTdataStruct, add4RxReceiveOkThenAdd1RxToFail)
{
	uint8_t i=0;
	for (i=0;i<4;i++)
	{
		setFrame(i);
		RT_command1CreateRequest((Command1RequestDataStruct *)&(uartRxProcess.node[i].buff[3]));
		TEST_ASSERT_EQUAL(STATE_ON, RT_uartRxPush(10, &uartRxProcess));
	}
	TEST_ASSERT_EQUAL(STATE_OFF, RT_uartRxPush(10, &uartRxProcess));

}

TEST(RTdataStruct, TestFrameStartAtCommand2)
{
	RTdataNode uartNode;
	ptrRTdataNode ptrNode=&uartNode;
	ptrNode->length = 10;
	ptrNode->buff = &(uartRxProcess.node[0].buff[0]);
	setFrame(0);
	RT_command2CreateRequest((Command2RequestDataStruct *)&(uartRxProcess.node[0].buff[3]));
	TEST_ASSERT_EQUAL(STATE_ON, RT_uartRxPush(10, &uartRxProcess));

	TEST_ASSERT_EQUAL(STATE_ON, RT_checkFrameStart(ptrNode));
}

TEST(RTdataStruct, TestFrameEndAtCommand3)
{
	UartNodeRx_t uartNode;
	ptrRTdataNode ptrNode=(ptrRTdataNode)&uartNode;
	ptrNode->length = 10;
	ptrNode->buff=uartRxProcess.node[0].buff;
	setFrame(0);
	RT_command3CreateRequest((Command3RequestDataStruct *)&(uartRxProcess.node[0].buff[3]));
	TEST_ASSERT_EQUAL(STATE_ON, RT_uartRxPush(10, &uartRxProcess));

	TEST_ASSERT_EQUAL(STATE_ON, RT_checkFrameStart(ptrNode));
	TEST_ASSERT_EQUAL(STATE_ON, RT_checkFrameEnd(ptrNode));
}

