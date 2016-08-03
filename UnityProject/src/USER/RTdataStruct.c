#include "cominc.h"

#include<stdio.h>

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

uint8_t RT_getNodesCount(ptrUartNodesProcess nodes)
{
	return ((nodes->max - nodes->in) + (nodes->out));
}

void RT_addOutIndex(ptrUartNodesProcess nodes)
{
	nodes->out++;
	if (nodes->out>=nodes->max)
	{
		nodes->out=0;
	}
}


//check frame start 
uint8_t RT_checkFrameStart(ptrRTdataNode ptrNode)
{
	uint8_t i,len=ptrNode->length;
	uint8_t *buf=&(ptrNode->buff[0]);
	for (i=0; i<((len>>1) + 1); i++)
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
	txAddr[dataStartBit+len]=len+funcode;  //У����
	len += 3; //���ճ���+3(֡ͷ1��֡ͷ2��У���)
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
	txAddr[dataStartBit+len]=len+funcode;  //У����
	len += 3; //���ճ���+3(֡ͷ1��֡ͷ2��У���)
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

uint8_t RT_command4ReceiveRequest(Command4RequestDataStruct *ptrc4)
{
	return ptrc4->outKind;
}

uint16_t RT_command4SendReturn(int8_t *txAddr)
{
	static uint16_t i=1;
	uint16_t j=0;
	Command3ReturnDataStruct returnData;
	
	vQUEGetTemperParams(&returnData);

	j=sprintf((char *)txAddr, "i %d,",i);
	j += sprintf((char *)(txAddr+j), " WIn %d,",returnData.waterIn);
//	j += sprintf((char *)(txAddr+j), " wOut %d,",returnData.waterOut);
	j += sprintf((char *)(txAddr+j), " wBank %d,",returnData.waterBank);
	j += sprintf((char *)(txAddr+j), " eviT %d,",returnData.evironT);
	j += sprintf((char *)(txAddr+j), " innerT %d,",returnData.innerTemper);

	j += sprintf((char *)(txAddr+j), " airevaT %d,",returnData.machineA.evaporateTemper);
	j += sprintf((char *)(txAddr+j), " airinT %d,",returnData.machineA.inTemper);
	j += sprintf((char *)(txAddr+j), " airOutT %d,",returnData.machineA.outTemper);

	j += sprintf((char *)(txAddr+j), " errType %d,",returnData.errType);
	j += sprintf((char *)(txAddr+j), " cdIostate %x,",returnData.cd4051DectState);

	j += sprintf((char *)(txAddr+j), " mstate %d",returnData.machineA.state);//..........
	j += sprintf((char *)(txAddr+j), " valMAstep %d,",returnData.machineA.valveMainStep);
	j += sprintf((char *)(txAddr+j), " valSBstep %d\r\n",returnData.machineA.valveSubStep);


	i++;
	return j;
}


void RT_command5ReceiveRequest(Command5RequestDataStruct *ptrc5)
{
	vRelay_command5SetRealy(ptrc5->data1Relay);
	ValveCalc_command5PushSig(ptrc5->data2ValveA, ValveMainA);
	ValveCalc_command5PushSig(ptrc5->data3ValveB, ValveSubB);
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

void RT_command4CreateRequest(Command4RequestDataStruct *ptrc4)
{
	uint8_t adddata=0;
	uint8_t *buf = (uint8_t *)ptrc4;
	uint8_t len=sizeof(Command4RequestDataStruct);

	ptrc4->dataFrame.length = len;
	ptrc4->dataFrame.command = FunCode4;
	ptrc4->outKind = 1;
	adddata = RT_dataAdd(len,buf);
	buf[len] = adddata;
}

void RT_command5CreateRequest(Command5RequestDataStruct *ptrc5, Command5RequestDataStruct *ptrc5Data)
{
	uint8_t adddata=0;
	uint8_t *buf = (uint8_t *)ptrc5;
	uint8_t len=sizeof(Command5RequestDataStruct);

	ptrc5->dataFrame.length = len;
	ptrc5->dataFrame.command = FunCode5;

	ptrc5->data1Relay = ptrc5Data->data1Relay;
	ptrc5->data2ValveA = ptrc5Data->data2ValveA;
	ptrc5->data3ValveB = ptrc5Data->data3ValveB;
	adddata = RT_dataAdd(len,buf);
	buf[len] = adddata;
}
