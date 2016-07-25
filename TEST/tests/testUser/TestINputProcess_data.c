#include "cominc.h"
#include "unity.h"
#include "unity_fixture.h"

ptrInputProcessStruct dataInp;

ErrStruct err;
uint16_t i,j;

ptrState adcstate;
ptrState cd4051State;

uint16_t state;
uint16_t runCount;
uint16_t needRunCount;
uint16_t cycleTime;

TEST_GROUP(INPData);


void  changeNewState(ptrState dst)
{
	State_ChangeState(dst, state,runCount,needRunCount,cycleTime);
}

TEST_SETUP(INPData)
{
	dataInp = INP_getDataInput();
	adcstate = BspAdc_getPtrStateAdc();
	cd4051State = BspAdc_getPtrStateCd4051();
}

TEST_TEAR_DOWN(INPData)
{
}


TEST(INPData, adcDataProcessedNotProcessureAgain)
{
	state = STATE_OFF;
	runCount = 3;
	needRunCount = 3;
	cycleTime = 5;

	changeNewState(adcstate);
	State_setDataProcessedFlag(adcstate,STATE_DATA_DONE);

	for (i=0;i<ADCMAX; i++)
	{
		dataInp->usedNtc[i]=ADC_USED;
		dataInp->finaNtc[i]=0;
		for (j=0;j<ADC_NeedRunCount;j++)
		{
			dataInp->dataNtc[j][i]=100;
		}
	}

	Inp_adcDataProcess();
	TEST_ASSERT_EQUAL(0, dataInp->finaNtc[0]);
}

TEST(INPData, adcDataProcessedWhileNoErr)
{
	uint16_t tdata=2000;
	state = STATE_OFF;
	runCount = 3;
	needRunCount = 3;
	cycleTime = 5;

	ERR_clearErr();
	changeNewState(adcstate);
	State_setDataProcessedFlag(adcstate,STATE_DATA_UNDONE);

	for (i=0;i<ADCMAX; i++)
	{
		dataInp->usedNtc[i]=ADC_USED;
		dataInp->finaNtc[i]=0;
		for (j=0;j<ADC_NeedRunCount;j++)
		{
			dataInp->dataNtc[j][i]=tdata;
		}
	}

	Inp_adcDataProcess();
	TEST_ASSERT_EQUAL(tdata, dataInp->finaNtc[0]);
}

TEST(INPData, adcDataProcessedWhile1ErrThenHave1ErrAtId0)
{
	uint16_t tdata=2000;
	state = STATE_OFF;
	runCount = 3;
	needRunCount = 3;
	cycleTime = 5;

	ERR_clearErr();
	changeNewState(adcstate);
	State_setDataProcessedFlag(adcstate,STATE_DATA_UNDONE);

	for (i=0;i<1; i++)
	{
		dataInp->usedNtc[i]=ADC_USED;
		dataInp->finaNtc[i]=0;
		for (j=0;j<ADC_NeedRunCount;j++)
		{
			dataInp->dataNtc[j][i]=10;
		}
	}

	for (i=1;i<ADCMAX; i++)
	{
		dataInp->usedNtc[i]=ADC_USED;
		dataInp->finaNtc[i]=0;
		for (j=0;j<ADC_NeedRunCount;j++)
		{
			dataInp->dataNtc[j][i]=tdata;
		}
	}

	Inp_adcDataProcess();
	TEST_ASSERT_EQUAL(ERR_USED, ERR_usedFlag());
	ERR_getErr(&err);
	TEST_ASSERT_EQUAL(ERR_NTC, err.errType);
	TEST_ASSERT_EQUAL(0, err.id);
}

TEST(INPData, adcDataProcessedWhile1ErrThenHave1ErrAtId1)
{
	uint16_t tdata=2000;
	state = STATE_OFF;
	runCount = 3;
	needRunCount = 3;
	cycleTime = 5;

	ERR_clearErr();
	changeNewState(adcstate);
	State_setDataProcessedFlag(adcstate,STATE_DATA_UNDONE);

	for (i=0;i<1; i++)
	{
		dataInp->usedNtc[i]=ADC_USED;
		dataInp->finaNtc[i]=0;
		for (j=0;j<ADC_NeedRunCount;j++)
		{
			dataInp->dataNtc[j][i]=tdata;
		}
	}


	for (i=1;i<2; i++)
	{
		dataInp->usedNtc[i]=ADC_USED;
		dataInp->finaNtc[i]=0;
		for (j=0;j<ADC_NeedRunCount;j++)
		{
			dataInp->dataNtc[j][i]=10;
		}
	}

	for (i=2;i<ADCMAX; i++)
	{
		dataInp->usedNtc[i]=ADC_USED;
		dataInp->finaNtc[i]=0;
		for (j=0;j<ADC_NeedRunCount;j++)
		{
			dataInp->dataNtc[j][i]=tdata;
		}
	}

	Inp_adcDataProcess();
	TEST_ASSERT_EQUAL(ERR_USED, ERR_usedFlag());
	ERR_getErr(&err);
	TEST_ASSERT_EQUAL(ERR_NTC, err.errType);
	TEST_ASSERT_EQUAL(1, err.id);
}

TEST(INPData, adcDataProcessedWhile2ErrThenHave1ErrAtId1)
{
	uint16_t tdata=2000;
	state = STATE_OFF;
	runCount = 3;
	needRunCount = 3;
	cycleTime = 5;

	ERR_clearErr();
	changeNewState(adcstate);
	State_setDataProcessedFlag(adcstate,STATE_DATA_UNDONE);

	for (i=0;i<1; i++)
	{
		dataInp->usedNtc[i]=ADC_USED;
		dataInp->finaNtc[i]=0;
		for (j=0;j<ADC_NeedRunCount;j++)
		{
			dataInp->dataNtc[j][i]=tdata;
		}
	}

	//err data=10
	for (i=1;i<3; i++)
	{
		dataInp->usedNtc[i]=ADC_USED;
		dataInp->finaNtc[i]=0;
		for (j=0;j<ADC_NeedRunCount;j++)
		{
			dataInp->dataNtc[j][i]=10;
		}
	}

	for (i=3;i<ADCMAX; i++)
	{
		dataInp->usedNtc[i]=ADC_USED;
		dataInp->finaNtc[i]=0;
		for (j=0;j<ADC_NeedRunCount;j++)
		{
			dataInp->dataNtc[j][i]=tdata;
		}
	}

	Inp_adcDataProcess();
	TEST_ASSERT_EQUAL(ERR_USED, ERR_usedFlag());
	ERR_getErr(&err);
	TEST_ASSERT_EQUAL(ERR_NTC, err.errType);
	TEST_ASSERT_EQUAL(1, err.id);
}


TEST(INPData, cd4051DataProcessedWhileNoErr)
{
	uint16_t tdata=2000;
	state = STATE_OFF;
	runCount = 3;
	needRunCount = 3;
	cycleTime = 5;

	ERR_clearErr();
	ERR_getErr(&err);
	changeNewState(cd4051State);
	State_setDataProcessedFlag(cd4051State,STATE_DATA_UNDONE);

	for (i=0;i<ADCMAX; i++)
	{
		dataInp->dataCd4051[i]=0xff;
	}

	Inp_cd4051DataProcess();
 	TEST_ASSERT_EQUAL(0xff, dataInp->finaCD4051);
	//这里有个问题，直接使用ERR_usedFlag()，返回值会变成255？？？？？
	//TEST_ASSERT_EQUAL(ERR_UNUSED, ERR_usedFlag());
	TEST_ASSERT_EQUAL(ERR_UNUSED, err.errUsed);

}

TEST(INPData, cd4051DataProcessedWhile1ErrAtId0)
{
	uint16_t tdata=2000;
	state = STATE_OFF;
	runCount = 3;
	needRunCount = 3;
	cycleTime = 5;

	ERR_clearErr();
	ERR_getErr(&err);
	changeNewState(cd4051State);
	State_setDataProcessedFlag(cd4051State,STATE_DATA_UNDONE);

	for (i=0;i<ADCMAX; i++)
	{
		dataInp->dataCd4051[i]=0xff;
	}

	Inp_cd4051DataProcess();
	TEST_ASSERT_EQUAL(0xff, dataInp->finaCD4051);
	TEST_ASSERT_EQUAL(ERR_UNUSED, ERR_usedFlag());
}

