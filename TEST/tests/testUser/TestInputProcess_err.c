#include "cominc.h"
#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP(INP_err);
ptrInputProcessStruct dataInp;

ErrStruct err;
uint16_t i;

TEST_SETUP(INP_err)
{
	dataInp = INP_getDataInput();
}

TEST_TEAR_DOWN(INP_err)
{

}

TEST(INP_err, Ntc_ErrusedButTypeNotErrntc)
{
	ERR_setErr(ERR_HIGH_PREESURE,0);

	TEST_ASSERT_EQUAL(ERR_USED, Inp_adcGetErr());
	ERR_getErr(&err);
	TEST_ASSERT_EQUAL(ERR_HIGH_PREESURE, err.errType);
}

TEST(INP_err, Ntc_ErrusedAndTypeErrntcToNoErr)
{
	ERR_setErr(ERR_NTC,0);
	for (i=0;i<ADCMAX; i++)
	{
		dataInp->finaNtc[i]=200;
		dataInp->usedNtc[i]=ADC_USED;
	}
	TEST_ASSERT_EQUAL(ERR_UNUSED, Inp_adcGetErr());

	ERR_getErr(&err);
	TEST_ASSERT_EQUAL(ERR_UNUSED, err.errUsed);
}

TEST(INP_err, Ntc_ErrusedAndTypeErrntcToErr)
{
	ERR_setErr(ERR_NTC,0);
	for (i=0;i<ADCMAX; i++)
	{
		if (i>=2)
		{
			dataInp->finaNtc[i]=10;
		}
		else
			dataInp->finaNtc[i]=100;
		dataInp->usedNtc[i]=ADC_USED;
	}
	TEST_ASSERT_EQUAL(ERR_USED, Inp_adcGetErr());

	ERR_getErr(&err);
	TEST_ASSERT_EQUAL(2, err.id);
}

TEST(INP_err, Ntc_NoErrToErr)
{
	ERR_clearErr();
	for (i=0;i<ADCMAX; i++)
	{
		if (i>=2)
		{
			dataInp->finaNtc[i]=10;
		}
		else
			dataInp->finaNtc[i]=100;
		dataInp->usedNtc[i]=ADC_USED;
	}
	TEST_ASSERT_EQUAL(ERR_USED, Inp_adcGetErr());

	ERR_getErr(&err);
	TEST_ASSERT_EQUAL(2, err.id);
}

TEST(INP_err, Ntc_NoErrToNoErr)
{
	ERR_clearErr();
	for (i=0;i<ADCMAX; i++)
	{
		dataInp->finaNtc[i]=100;
		dataInp->usedNtc[i]=ADC_USED;
	}
	TEST_ASSERT_EQUAL(ERR_UNUSED, Inp_adcGetErr());

	ERR_getErr(&err);
	TEST_ASSERT_EQUAL(ERR_UNUSED, err.errUsed);
}

