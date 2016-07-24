#include "cominc.h"
#define MAX_StopTimes 3

uint8_t waterCheckFlag=0;

IODectectErrStruct ioErrDectect[IOERR_MAX] ={
	{ERR_LOW_PREESURE,IOERR_OFF,0,0,MAX_StopTimes,0},
	{ERR_HIGH_PREESURE,IOERR_OFF,0,0,MAX_StopTimes,0},
	{ERR_WATER_OPEN,IOERR_OFF,0,0,MAX_StopTimes,0},
	{ERR_TEMPERA110,IOERR_OFF,0,0,MAX_StopTimes,0},
};

ptrIODectectErr IODect_getStructAddress(void)
{
	return &ioErrDectect[0];
}

void IODECT_setIoDetectData(ptrIODectectErr dstErrAdrees, ptrIODectectErr srcErrAdrees)
{
//s	dstErrAdrees->errDetectType = srcErrAdrees->errDetectType ;
	dstErrAdrees->state = srcErrAdrees->state;
	dstErrAdrees->stopTimes = srcErrAdrees->stopTimes;
	dstErrAdrees->runCount = srcErrAdrees->runCount;
	dstErrAdrees->MaxRunCount = srcErrAdrees->MaxRunCount;
	dstErrAdrees->errCount = srcErrAdrees->errCount;
}

ptrIODectectErr findIoDetectAddress(ErrType errType)
{
	uint8_t i;
	for(i=0;i<IOERR_MAX; i++)
	{
		if (ioErrDectect[i].errDetectType == errType)
		{
			return &ioErrDectect[i];
		}
	}
	return NULL;
}

void ioDetectCheckErr(ptrIODectectErr ioErrAddress,uint16_t inXData)
{
	if (ioErrAddress->stopTimes >= MAX_StopTimes)
	{
		//set an err
		ERR_setErr(ioErrAddress->errDetectType, 0);
	}
	else{
		if (inXData == CDIN_OPEN)
		{
			if (ioErrAddress->state == IOERR_OFF)
			{
				ioErrAddress->state = IOERR_ON;
			}

			ioErrAddress->errCount ++;
		}
		ioErrAddress->runCount++;
		if (ioErrAddress->runCount >= ioErrAddress->MaxRunCount)
		{
			//错误次数占一半
			//errCount=0时， 0>>1=0,此处只能写大于
			if (ioErrAddress->errCount > (ioErrAddress->errCount>>1))
			{
				//send an shout down message
				//set stoptimes +1
				//set runCount,errCount to 0
				ioErrAddress->stopTimes++;
				ioErrAddress->runCount = 0;
				ioErrAddress->errCount = 0;
			}
			else
			{
				//本次错误次数少于一半
				//1.set state to stateOff
				//2.set stopTime,runCount,errCount to 0
				ioErrAddress->stopTimes=0;
				ioErrAddress->runCount = 0;
				ioErrAddress->errCount = 0;
				ioErrAddress->state = IOERR_OFF;
			}
		}
	}
}

void IOERR_checkErr(ErrType errType, uint16_t inXData)
{
	ErrStruct err;
	
	//1.get address
	ptrIODectectErr ioErrAddress=findIoDetectAddress(errType);
	if (ioErrAddress == NULL)
	{
		return ;
	}

	//2.set check is there alread have an error
	ERR_getErr(&err);
	if (err.errUsed == ERR_USED && err.errType!=errType)
	{
		return ;
	}
	//3.inData != open and state ==off ,means no error, rreturn 
	if (inXData != CDIN_OPEN && ioErrAddress->state == IOERR_OFF)
	{
		return;
	}
	//4. an error chenck
	ioDetectCheckErr(ioErrAddress,inXData);
	return ;
}

void IODECT_startCheckWaterOpen(void)
{
	waterCheckFlag=STATE_ON;
}

void IODECT_stopCheckWaterOpen(void)
{
	waterCheckFlag=STATE_OFF;
	//IOERR_OFF,0,0,MAX_StopTimes,0
	ioErrDectect[2].state = IOERR_OFF;
	ioErrDectect[2].stopTimes =0;
	ioErrDectect[2].runCount = 0;
	ioErrDectect[2].MaxRunCount=MAX_StopTimes;
	ioErrDectect[2].errCount = 0;
}

uint8_t IODECT_getFlagCheckWaterOpen(void)
{
	return waterCheckFlag;
}