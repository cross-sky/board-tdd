#include "cominc.h"
#include "unity.h"
#include "unity_fixture.h"

#include "Stm32_ub_adc_spy.h"

static UartProcess_t uartRxProcess={
	0
};

uint8_t *Txbuff;
static void setFrame(uint8_t i)
{
	uartRxProcess.node[i].length = 10;
	uartRxProcess.node[i].buff[0]=0xfe;
	uartRxProcess.node[i].buff[1]=0xfe;
	uartRxProcess.node[i].buff[2]=0xef;
}


TEST_GROUP(Uart1Dma);

TEST_SETUP(Uart1Dma)
{
	memset(&uartRxProcess,0,sizeof(UartProcess_t));
	uartRxProcess.max = 5;
	Txbuff = vUart_getTxBuff();
}

TEST_TEAR_DOWN(Uart1Dma)
{

}


TEST(Uart1Dma, RxCommand1ReturnOK)
{
	RTdataNode uartNode;
	uartNode.length = 10;
	uartNode.buff = &(uartRxProcess.node[0].buff[0]);
	setFrame(0);
	RT_command1CreateRequest((Command1RequestDataStruct *)&(uartRxProcess.node[0].buff[3]));
	TEST_ASSERT_EQUAL(STATE_ON, RT_uartRxPush(10, &uartRxProcess));
	
	vUartRxPopProcess(&uartRxProcess);
	vUartDmaTxHandler_ISR();
	TEST_ASSERT_EQUAL(FunCode1, Txbuff[3]);
}

TEST(Uart1Dma, RxCommand2ReturnOK)
{
	RTdataNode uartNode;
	uartNode.length = 10;
	uartNode.buff = &(uartRxProcess.node[0].buff[0]);
	setFrame(0);
	RT_command2CreateRequest((Command2RequestDataStruct *)&(uartRxProcess.node[0].buff[3]));
	TEST_ASSERT_EQUAL(STATE_ON, RT_uartRxPush(10, &uartRxProcess));

	vUartRxPopProcess(&uartRxProcess);
	vUartDmaTxHandler_ISR();
	TEST_ASSERT_EQUAL(FunCode2, Txbuff[3]);
}

TEST(Uart1Dma, RxCommand3ReturnOK)
{
	RTdataNode uartNode;
	uartNode.length = 10;
	uartNode.buff = &(uartRxProcess.node[0].buff[0]);
	setFrame(0);
	RT_command3CreateRequest((Command3RequestDataStruct *)&(uartRxProcess.node[0].buff[3]));
	TEST_ASSERT_EQUAL(STATE_ON, RT_uartRxPush(10, &uartRxProcess));

	vUartRxPopProcess(&uartRxProcess);
	vUartDmaTxHandler_ISR();
	TEST_ASSERT_EQUAL(FunCode3, Txbuff[3]);
}