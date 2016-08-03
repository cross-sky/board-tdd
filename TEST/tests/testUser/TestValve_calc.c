#include "cominc.h"
#include "unity.h"
#include "unity_fixture.h"

#define VALVE_STEPSInit	16	//默认每次运行最大步数

static ValveStatus_t testValveData[ValveKindsMax]={
	{statusDone,0,0,VALVE_STEPSInit,DirectHold,0},
	{statusDone,0,0,VALVE_STEPSInit,DirectHold,0}
};
ValveProcess_t *valveSigs;
ValveSig_t sig;
CoreProcess_t *queData;

static void setInitTestValveData(uint16_t valveKind)
{
	testValveData[valveKind].runStep = 0;
	testValveData[valveKind].totalSteps = 0;
	testValveData[valveKind].valveStatus = statusDone;
	testValveData[valveKind].lastStep = VALVE_STEPSInit;
	testValveData[valveKind].valveDirection = DirectHold;
	testValveData[valveKind].valveCounts = 0;
}

void setQueDataInEvaTemper(int16_t inT,int16_t evaT)
{
	queData->coreParems.machineA.inTemper = inT*10;
	queData->coreParems.machineA.evaporateTemper = evaT*10;
}

TEST_GROUP(ValveCalc);


TEST_SETUP(ValveCalc)
{
	ValveCalv_changeValveData(testValveData);
	 valveSigs = ValveCalc_getSigNodes();
	 valveSigs->in = 0;
	 valveSigs->out = 0;
	 queData = xQue_getCoreData();
}

TEST_TEAR_DOWN(ValveCalc)
{

}

TEST(ValveCalc, PopAndPushSig)
{
	ValveSig_t tsig;
	sig.code = -10;
	sig.sig = valveInit;
	sig.kindValue = ValveMainA;
	ValveCalc_pushSig(&sig);
	ValveCalc_popSig(&tsig);
	TEST_ASSERT_EQUAL(valveInit,tsig.sig);
}


TEST(ValveCalc, CheckTotalStep)
{
	sig.code = -10;
	sig.sig = valveInit;
	sig.kindValue = ValveMainA;

	checkTotalSteps(&sig);
	TEST_ASSERT_EQUAL(0,testValveData[ValveMainA].totalSteps);

	sig.code = -10;
	sig.sig = valveRun;
	checkTotalSteps(&sig);
	TEST_ASSERT_EQUAL(VALVE_MIN_STEP,testValveData[ValveMainA].totalSteps);

	sig.code = 500;
	sig.sig = valveRun;
	checkTotalSteps(&sig);
	TEST_ASSERT_EQUAL(VALVE_MAX_STEP,testValveData[ValveMainA].totalSteps);

	sig.code = -100;
	sig.sig = valveRun;
	checkTotalSteps(&sig);
	TEST_ASSERT_EQUAL(VALVE_MAX_STEP+sig.code,testValveData[ValveMainA].totalSteps);

	sig.code = 500;
	sig.sig = valveClose;
	checkTotalSteps(&sig);
	TEST_ASSERT_EQUAL(VALVE_CLOSE_STEP,testValveData[ValveMainA].totalSteps);

	sig.code = 10;
	sig.sig = valveInit;
	checkTotalSteps(&sig);
	TEST_ASSERT_EQUAL(0,testValveData[ValveMainA].totalSteps);
}

TEST(ValveCalc, valveCalcCheckProcess)
{
	uint16_t i;
	sig.code = 10;
	sig.sig = valveRun;
	sig.kindValue = ValveMainA;

	setInitTestValveData(0);

	ValveCalc_pushSig(&sig);
	ValveCalc_checkProcess(ValveMainA);
	
	for(i=0;i<VALVE_MIN_STEP ;i++)
	{
		ValveCalc_valveRun(ValveMainA);
	}

	//额外10次保持励磁
	for(i=0;i<=10 ;i++)
	{
		ValveCalc_valveRun(ValveMainA);
	}

	ValveCalc_checkProcess(ValveMainA);
	TEST_ASSERT_EQUAL(statusDone,testValveData[ValveMainA].valveStatus);
}

TEST(ValveCalc, valveCalcValveMainWhen4TimesForward)
{
	ValveSig_t tsig;

	//1times foward
	setQueDataInEvaTemper(10,5);
	setInitTestValveData(0);

	ValveCalc_calcValveMain(ValveMainA);
	ValveCalc_popSig(&tsig);

	TEST_ASSERT_EQUAL(valveRun,tsig.sig);
	TEST_ASSERT_EQUAL(VALVE_STEPSInit,tsig.code);

	//2times foward
	ValveCalc_calcValveMain(ValveMainA);
	ValveCalc_popSig(&tsig);

	TEST_ASSERT_EQUAL(VALVE_STEPSInit,tsig.code);

	//3times foward
	ValveCalc_calcValveMain(ValveMainA);
	ValveCalc_popSig(&tsig);

	TEST_ASSERT_EQUAL(VALVE_STEPSInit,tsig.code);

	//4times foward
	ValveCalc_calcValveMain(ValveMainA);
	ValveCalc_popSig(&tsig);

	TEST_ASSERT_EQUAL(VALVE_STEPSInit,tsig.code);

}

TEST(ValveCalc, valveCalcValveMainWhen4TimesBack)
{
	ValveSig_t tsig;

	//1times Back
	setQueDataInEvaTemper(6,5);
	setInitTestValveData(0);

	ValveCalc_calcValveMain(ValveMainA);
	ValveCalc_popSig(&tsig);

	TEST_ASSERT_EQUAL(VALVE_STEPSInit*(-1),tsig.code);

	//2times Back
	ValveCalc_calcValveMain(ValveMainA);
	ValveCalc_popSig(&tsig);

	TEST_ASSERT_EQUAL(VALVE_STEPSInit*(-1),tsig.code);

	//3times Back
	ValveCalc_calcValveMain(ValveMainA);
	ValveCalc_popSig(&tsig);

	TEST_ASSERT_EQUAL(VALVE_STEPSInit*(-1),tsig.code);

	//4times Back
	ValveCalc_calcValveMain(ValveMainA);
	ValveCalc_popSig(&tsig);

	TEST_ASSERT_EQUAL(VALVE_STEPSInit*(-1),tsig.code);

	TEST_ASSERT_EQUAL(0,testValveData[ValveMainA].valveCounts);
}

TEST(ValveCalc, valveCalcValveMainWhenForwardAndBack)
{
	ValveSig_t tsig;

	//1times foward
	setQueDataInEvaTemper(10,5);
	setInitTestValveData(0);

	ValveCalc_calcValveMain(ValveMainA);
	ValveCalc_popSig(&tsig);

	TEST_ASSERT_EQUAL(VALVE_STEPSInit,tsig.code);

	//2times Back
	setQueDataInEvaTemper(6,5);
	ValveCalc_calcValveMain(ValveMainA);
	ValveCalc_popSig(&tsig);

	TEST_ASSERT_EQUAL((VALVE_STEPSInit>>1)*(-1),tsig.code);
}
