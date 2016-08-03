#include "cominc.h"
#include "unity.h"
#include "unity_fixture.h"
#include <string.h>
#define VALVE_STEPSInit	16	//默认每次运行最大步数

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

static ValveStatus_t testValveData[ValveKindsMax]={
	{statusDone,0,0,VALVE_STEPSInit,DirectHold,0},
	{statusDone,0,0,VALVE_STEPSInit,DirectHold,0}
};
static void setInitTestValveData(uint16_t valveKind)
{
	testValveData[valveKind].runStep = 0;
	testValveData[valveKind].totalSteps = 0;
	testValveData[valveKind].valveStatus = statusDone;
	testValveData[valveKind].lastStep = VALVE_STEPSInit;
	testValveData[valveKind].valveDirection = DirectHold;
	testValveData[valveKind].valveCounts = 0;
}

TEST_GROUP(RTdataStruct);


TEST_SETUP(RTdataStruct)
{
	memset(&uartRxProcess,0,sizeof(UartProcess_t));
	uartRxProcess.max = 5;
	ValveCalv_changeValveData(testValveData);
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


TEST(RTdataStruct, TestCommand4)
{
	uint8_t buff[300];
	uint16_t len;
	len = RT_command4SendReturn(buff);
	printf("len is %d \r\n", len);
	printf("%s",buff);
}

TEST(RTdataStruct, TestCommand5)
{
	UartNodeRx_t uartNode;
	ptrRTdataNode ptrNode=(ptrRTdataNode)&uartNode;

	Command5RequestDataStruct c5Data;
	uint16_t data1Relay=0xfff;
	int8_t data2ValveA=30;
	int8_t data3ValveB = 30;

	c5Data.data1Relay =data1Relay;
	c5Data.data2ValveA = data2ValveA;
	c5Data.data3ValveB = data3ValveB;

	ptrNode->length = 10;
	ptrNode->buff=uartRxProcess.node[0].buff;
	setFrame(0);

	setInitTestValveData(0);
	setInitTestValveData(1);

	RT_command5CreateRequest((Command5RequestDataStruct *)&(uartRxProcess.node[0].buff[3]),&c5Data);

	RT_command5ReceiveRequest((Command5RequestDataStruct *)&(uartRxProcess.node[0].buff[3]));

	ValveCalc_checkProcess(ValveMainA);

	TEST_ASSERT_EQUAL(data2ValveA,testValveData[ValveMainA].totalSteps);
}
