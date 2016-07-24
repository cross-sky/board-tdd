#include "cominc.h"

uint8_t* puartGetRTxAddress(ptrUartNodesProcess nodes)
{
	return &(nodes->node[nodes->in].buff[RTX_START_BIT]);
}

uint8_t RT_uartRxPush(uint8_t len,ptrUartNodesProcess nodes)
{
	if ((nodes->in+1)%nodes->max == nodes->out)
	{
		return STATE_OFF;
	}
	nodes->node->length = len;
	nodes->in++;
	if (nodes->in >= nodes->max)
	{
		nodes->in=0;
	}
	return STATE_ON;
}


//check frame start 
uint8_t RT_checkFrameStart(ptrRTdataNode ptrNode)
{
	uint8_t i,len=ptrNode->length;
	uint8_t *buf=&(ptrNode->buff[0]);
	for (i=0; i<(len>>2); i++)
	{
		if (*(buf)==FrameStart1Code && *(buf+1)==FrameStart2Code)
		{
			break;
		}
		else
		{
			buf+=1;
			len-=1;
		}
	}

	if (*(buf)==FrameStart1Code && *(buf+1)==FrameStart2Code)
	{
		ptrNode->length = len;
		ptrNode->buff=buf+2; //data start at length
		
		return STATE_ON;
	}
	return STATE_OFF;
}

uint8_t RT_dataAdd(uint8_t len, uint8_t *addr)
{
	uint8_t i,adds=0;
	for (i=0;i<len; i++)
	{
		adds += addr[i];
	}
	return adds;
}

//************************************
// Method:    RT_checkFrameEnd
// FullName:  RT_checkFrameEnd
// Access:    public 
// Returns:   uint8_t
// Qualifier: check frame end, ptrNode.length should be the size of RxDataStruct.
// Parameter: ptrRTdataNode ptrNode
//************************************
uint8_t RT_checkFrameEnd(ptrRTdataNode ptrNode)
{
	uint8_t addData=0;

	addData = RT_dataAdd(ptrNode->buff[0], ptrNode->buff);
	//&& buf[i+1] == FrameEndCode
	if (addData == ptrNode->buff[ptrNode->buff[0]] )
	{
		return STATE_ON;
	}
	return STATE_OFF;
}

//receive command1 request
void RT_command1Receiverequest(Command1RequestDataStruct *ptrC1)
{
	if (ptrC1->onOrOff == STATE_ON)
	{
		xQUESigPush(SIG_ON);
	}
	else
		xQUESigPush(SIG_OFF);

	if(ptrC1->defrost == STATE_ON)
	{
		xQUESigPush(SIG_DEFROST);
	}
}

//Return command1 request
uint8_t RT_command12SendReturn(uint8_t funcode,uint8_t *txAddr)
{
	uint8_t dataStartBit = 2;
	uint8_t len;
	Command12ReturnDataStruct *returnData=(Command12ReturnDataStruct *)&(txAddr[dataStartBit]);
	
	returnData->dataFrame.command = funcode;
	len = sizeof(Command12ReturnDataStruct);
	returnData->dataFrame.length = len;
	txAddr[dataStartBit+len]=len+funcode;  //校验码
	len += 3; //最终长度+3(帧头1，帧头2，校验符)
	return len;
}

uint8_t RT_command3SendReturn(uint8_t funcode,uint8_t *txAddr)
{
	uint8_t dataStartBit = 2;
	uint8_t len;
	Command3ReturnDataStruct *returnData=(Command3ReturnDataStruct *)&(txAddr[dataStartBit]);
	vQUEGetTemperParams(returnData);

	returnData->dataFrame.command = funcode;
	len = sizeof(Command3ReturnDataStruct);
	returnData->dataFrame.length = len;
	txAddr[dataStartBit+len]=len+funcode;  //校验码
	len += 3; //最终长度+3(帧头1，帧头2，校验符)
	return len;

}

void RT_command2ReceiveRequest(Command2RequestDataStruct *ptrC2)
{
	vQueSetCoreParams(ptrC2);
}

void RT_command3ReceiveRequest(void)
{
	//do nothinf;
}


void RT_command1CreateRequest(Command1RequestDataStruct *ptrc1)
{
	uint8_t adddata=0;
	uint8_t *buf = (uint8_t *)ptrc1;
	ptrc1->dataFrame.length = sizeof(Command1RequestDataStruct);
	ptrc1->dataFrame.command = FunCode1;
	ptrc1->onOrOff = 1;
	ptrc1->defrost = 1;

	adddata = RT_dataAdd(ptrc1->dataFrame.length, buf);
	buf[ptrc1->dataFrame.length] = adddata;
}

void RT_command2CreateRequest(Command2RequestDataStruct *ptrc2)
{
	uint8_t adddata=0;
	uint8_t *buf = (uint8_t *)ptrc2;
	uint8_t len=sizeof(Command2RequestDataStruct);

	ptrc2->dataFrame.length = len;
	ptrc2->dataFrame.command = FunCode2;
	ptrc2->setWaterT = 50;
	ptrc2->setWaterCycleTemper = 5;
	ptrc2->setDefrostInTemper = -1;
	ptrc2->setDefrostRunTimes = 8;
	ptrc2->setDefrostOutTemper = 10;
	ptrc2->setDefrostCycleTimes = 50;
	ptrc2->setDefrostInOffset = 6;

	adddata = RT_dataAdd(len,buf);
	buf[len] = adddata;
}

void RT_command3CreateRequest(Command3RequestDataStruct *ptrc3)
{
	uint8_t adddata=0;
	uint8_t *buf = (uint8_t *)ptrc3;
	uint8_t len=sizeof(Command3RequestDataStruct);

	ptrc3->dataFrame.length = len;
	ptrc3->dataFrame.command = FunCode3;
	adddata = RT_dataAdd(len,buf);
	buf[len] = adddata;
}
