#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP_RUNNER(INP_err)
{
	RUN_TEST_CASE(INP_err, Ntc_ErrusedButTypeNotErrntc);
	RUN_TEST_CASE(INP_err, Ntc_ErrusedAndTypeErrntcToNoErr);
	RUN_TEST_CASE(INP_err, Ntc_ErrusedAndTypeErrntcToErr);
	RUN_TEST_CASE(INP_err, Ntc_NoErrToErr);
	RUN_TEST_CASE(INP_err, Ntc_NoErrToNoErr);
}