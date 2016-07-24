#include "cominc.h"
#include "unity.h"
#include "unity_fixture.h"

ErrStruct err;
ptrIODectectErr dstIoDectectErr;
IODectectErrStruct srcIoDetectErr;
uint16_t i;

uint16_t MaxRunCount = 3;

void setSrcIoDetectErrData(void)
{
	srcIoDetectErr.state = IOERR_OFF;
	srcIoDetectErr.stopTimes = 0;
	srcIoDetectErr.runCount = 0;
	srcIoDetectErr.MaxRunCount = MaxRunCount;
	srcIoDetectErr.errCount = 0;
	IODECT_setIoDetectData(&dstIoDectectErr[0],&srcIoDetectErr);
	IODECT_setIoDetectData(&dstIoDectectErr[1],&srcIoDetectErr);
	IODECT_setIoDetectData(&dstIoDectectErr[2],&srcIoDetectErr);
}

TEST_GROUP(User_IODetectErr);


TEST_SETUP(User_IODetectErr)
{
	dstIoDectectErr = IODect_getStructAddress();
}

TEST_TEAR_DOWN(User_IODetectErr)
{

}


TEST(User_IODetectErr, alreadHaveAnErrNotDetectErr)
{
	ERR_setErr(ERR_HIGH_PREESURE,0);
	setSrcIoDetectErrData();
	
	IOERR_checkErr(ERR_LOW_PREESURE,CDIN_OPEN);
	//ERR_LOW_PREESURE 对应于dstIoDectectErr[0]，具体以IODetectErr.c中定义
	TEST_ASSERT_EQUAL(IOERR_OFF, dstIoDectectErr[0].state);
}

TEST(User_IODetectErr, noErrThenDetect1stErrAtAddress0)
{
	ERR_clearErr();
	setSrcIoDetectErrData();
	for (i=0; i<MaxRunCount;i++)
	{
		IOERR_checkErr(ERR_LOW_PREESURE,CDIN_OPEN);
	}
	//ERR_LOW_PREESURE 对应于dstIoDectectErr[0]，具体以IODetectErr.c中定义
	TEST_ASSERT_EQUAL(IOERR_ON, dstIoDectectErr[0].state);
	TEST_ASSERT_EQUAL(1, dstIoDectectErr[0].stopTimes);
}

TEST(User_IODetectErr, noErrThenDetect2stErrAtAddress0)
{
	ERR_clearErr();
	setSrcIoDetectErrData();
	for (i=0; i<MaxRunCount*2;i++)
	{
		IOERR_checkErr(ERR_LOW_PREESURE,CDIN_OPEN);
	}
	//ERR_LOW_PREESURE 对应于dstIoDectectErr[0]，具体以IODetectErr.c中定义
	TEST_ASSERT_EQUAL(IOERR_ON, dstIoDectectErr[0].state);
	TEST_ASSERT_EQUAL(2, dstIoDectectErr[0].stopTimes);
}

TEST(User_IODetectErr, noErrThenDetect3stErrAtAddress0)
{
	ERR_clearErr();
	setSrcIoDetectErrData();
	for (i=0; i<MaxRunCount*3;i++)
	{
		IOERR_checkErr(ERR_LOW_PREESURE,CDIN_OPEN);
	}
	//ERR_LOW_PREESURE 对应于dstIoDectectErr[0]，具体以IODetectErr.c中定义
	TEST_ASSERT_EQUAL(IOERR_ON, dstIoDectectErr[0].state);
	TEST_ASSERT_EQUAL(3, dstIoDectectErr[0].stopTimes);
}

TEST(User_IODetectErr, noErrThenDetect4stErrAtAddress0)
{
	ERR_clearErr();
	setSrcIoDetectErrData();
	for (i=0; i<MaxRunCount*3;i++)
	{
		IOERR_checkErr(ERR_LOW_PREESURE,CDIN_OPEN);
	}
	//ERR_LOW_PREESURE 对应于dstIoDectectErr[0]，具体以IODetectErr.c中定义
	TEST_ASSERT_EQUAL(IOERR_ON, dstIoDectectErr[0].state);
	TEST_ASSERT_EQUAL(3, dstIoDectectErr[0].stopTimes);
}

TEST(User_IODetectErr, noErrThenDetect1stErrAnd1stNoErrAtAddress0)
{
	ERR_clearErr();
	setSrcIoDetectErrData();
	for (i=0; i<MaxRunCount;i++)
	{
		IOERR_checkErr(ERR_LOW_PREESURE,CDIN_OPEN);
	}
	for (i=0; i<MaxRunCount;i++)
	{
		IOERR_checkErr(ERR_LOW_PREESURE,1);
	}
	//ERR_LOW_PREESURE 对应于dstIoDectectErr[0]，具体以IODetectErr.c中定义
	TEST_ASSERT_EQUAL(IOERR_OFF, dstIoDectectErr[0].state);
	TEST_ASSERT_EQUAL(0, dstIoDectectErr[0].stopTimes);
	setSrcIoDetectErrData();
}
