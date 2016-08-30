#include "cominc.h"
#include "unity.h"
#include "unity_fixture.h"

#define VALVE_STEPSInit	32	//默认每次运行最大步数

static ValveStatus_t testValveData[ValveKindsMax]={
	{statusDone,0,0,VALVE_STEPSInit,DirectHold,0},
	{statusDone,0,0,VALVE_STEPSInit,DirectHold,0}
};
ValveProcess_t *valveSigs;
ValveSig_t sig;
CoreProcess_t *queData;

int16_t getValveTotalStep(uint16_t valveKind)
{
	return testValveData[valveKind].totalSteps;
}

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

void setQueDataWaterAiroutEviTemper(int16_t waterT,int16_t airOutT,int16_t envirT)
{
	queData->coreParems.waterBank = waterT*10;
	queData->coreParems.environT = envirT*10;
	queData->coreParems.machineA.outTemper = airOutT*10;
}

void setQueDataAirOutT(int16_t airoutT)
{
	queData->coreParems.machineA.outTemper = airoutT*10;
}

void setQueDataColdModelEvaT(int16_t coldEvaT)
{
	queData->coreParems.waterIn = coldEvaT * 10;
}

TEST_GROUP(ValveCalc);


TEST_SETUP(ValveCalc)
{
	ValveCalv_changeValveData(testValveData);
	 valveSigs = ValveCalc_getSigNodes();
	 valveSigs->in = 0;
	 valveSigs->out = 0;
	 queData = xQue_getCoreData();
	 iQUE_setWorkerModel(SIG_MAKE_HotWater);
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
	TEST_ASSERT_EQUAL(0,testValveData[ValveMainA].totalSteps);

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
	setQueDataInEvaTemper(12,5);
	setInitTestValveData(0);
	setQueDataAirOutT(90);

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
	setQueDataAirOutT(90);

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
	setQueDataInEvaTemper(12,5);
	setInitTestValveData(0);
	setQueDataAirOutT(90);

	ValveCalc_calcValveMain(ValveMainA);
	ValveCalc_popSig(&tsig);

	TEST_ASSERT_EQUAL(VALVE_STEPSInit,tsig.code);

	//2times Back
	setQueDataInEvaTemper(6,5);
	ValveCalc_calcValveMain(ValveMainA);
	ValveCalc_popSig(&tsig);

	TEST_ASSERT_EQUAL((VALVE_STEPSInit>>1)*(-1),tsig.code);
}

TEST(ValveCalc, valveCalcValveMainWhenBackAndForward)
{
	ValveSig_t tsig;
	setInitTestValveData(0);
	setQueDataAirOutT(90);

	//1times Back
	setQueDataInEvaTemper(6,5);
	ValveCalc_calcValveMain(ValveMainA);
	ValveCalc_popSig(&tsig);

	TEST_ASSERT_EQUAL((VALVE_STEPSInit)*(-1),tsig.code);

	//2times foward
	setQueDataInEvaTemper(12,5);

	ValveCalc_calcValveMain(ValveMainA);
	ValveCalc_popSig(&tsig);

	TEST_ASSERT_EQUAL(VALVE_STEPSInit>>1,tsig.code);
}

TEST(ValveCalc, TestValveCalc_calcValveSub)
{
	uint16_t i;
	ValveSig_t tsig;
	int16_t water;
	int16_t airout;
	int16_t envir;

	//airout>100
	water = 30;
	airout = 105;
	envir = 10;
	setQueDataWaterAiroutEviTemper(water, airout, envir);
	ValveCalc_calcValveSub(ValveSubB);
	ValveCalc_popSig(&tsig);
	TEST_ASSERT_EQUAL((airout-100)/2,tsig.code);

	//airout<100, envir>5
	water = 30;
	airout = 90;
	envir = 10;
	setInitTestValveData(ValveSubB);
	testValveData[ValveSubB].totalSteps = 10;
	setQueDataWaterAiroutEviTemper(water, airout, envir);
	ValveCalc_calcValveSub(ValveSubB);

	ValveCalc_checkProcess(ValveSubB);
	for(i=0;i<10 ;i++)
	{
		ValveCalc_checkProcess(ValveSubB);
	}
	
	TEST_ASSERT_EQUAL(0,testValveData[ValveSubB].totalSteps);
	

	//airout<100 envir<5 airout-water=26
	water = 30;
	airout = 56;
	envir = 4;
	setQueDataWaterAiroutEviTemper(water, airout, envir);
	ValveCalc_calcValveSub(ValveSubB);
	ValveCalc_popSig(&tsig);
	TEST_ASSERT_EQUAL((airout-water-20)/2,tsig.code);

	//airout<100 envir<5 airout-water=30
	water = 30;
	airout = 60;
	envir = 4;
	setQueDataWaterAiroutEviTemper(water, airout, envir);
	ValveCalc_calcValveSub(ValveSubB);
	ValveCalc_popSig(&tsig);
	TEST_ASSERT_EQUAL(4,tsig.code);

	//airout<100 envir<5 airout-water=12
	water = 30;
	airout = 42;
	envir = 4;
	setQueDataWaterAiroutEviTemper(water, airout, envir);
	ValveCalc_calcValveSub(ValveSubB);
	ValveCalc_popSig(&tsig);
	TEST_ASSERT_EQUAL((airout-water - 16)/2,tsig.code);

	//airout<100 envir<5 airout-water=6
	water = 30;
	airout = 36;
	envir = 4;
	setQueDataWaterAiroutEviTemper(water, airout, envir);
	ValveCalc_calcValveSub(ValveSubB);
	//ValveCalc_popSig(&tsig);
	TEST_ASSERT_EQUAL(FALSE,ValveCalc_popSig(&tsig));

	//airout<100 envir<5 airout-water=18
	water = 30;
	airout = 48;
	envir = 4;
	setQueDataWaterAiroutEviTemper(water, airout, envir);
	ValveCalc_calcValveSub(ValveSubB);
	TEST_ASSERT_EQUAL(FALSE,ValveCalc_popSig(&tsig));
}

//test model change then init essential params
TEST(ValveCalc, TestValveCalc_HotwaterModelChangeToColdModel)
{
	ValveSig_t tsig;
	//1. change to cool model test max valvestep
	setInitTestValveData(ValveMainA);
	iQUE_setWorkerModel(SIG_MAKE_COLD);
	ValveCalc_WorkerModelChangeParams();

	ValveCalc_popSig(&tsig);
	//cold init step 30
	TEST_ASSERT_EQUAL(VALVE_MIN_STEP,tsig.code);

	tsig.code = 200;
	tsig.sig = valveRun;
	tsig.kindValue = ValveMainA;

	ValveCalc_pushSig(&tsig);
	ValveCalc_popSig(&tsig);
	checkTotalSteps(&tsig);
	//max step 120
	TEST_ASSERT_EQUAL(VALVE_COLDMAX_STEP,testValveData[ValveMainA].totalSteps);

	//change to hotwater model ,test max valvestep
	setInitTestValveData(ValveMainA);
	iQUE_setWorkerModel(SIG_MAKE_HotWater);
	ValveCalc_WorkerModelChangeParams();

	ValveCalc_popSig(&tsig);
	//hot init step 100
	TEST_ASSERT_EQUAL(VALVE_INITRUN_STEP,tsig.code);

	tsig.code = 500;
	tsig.sig = valveRun;
	tsig.kindValue = ValveMainA;

	ValveCalc_pushSig(&tsig);
	ValveCalc_popSig(&tsig);
	checkTotalSteps(&tsig);
	//max step 470
	TEST_ASSERT_EQUAL(VALVE_MAX_STEP,testValveData[ValveMainA].totalSteps);
}

TEST(ValveCalc, ColdModelValveCalcCheckProcess)
{
	uint16_t i;
	setInitTestValveData(ValveMainA);
	iQUE_setWorkerModel(SIG_MAKE_COLD);
	ValveCalc_WorkerModelChangeParams();

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


//cold back mean forward
TEST(ValveCalc, ColdModelValveCalcValveMainWhen4TimesBack4TimsFward)
{
	ValveSig_t tsig;
	int16_t water;
	int16_t airout;
	int16_t envir;

	water = 20;
	airout = 90;
	envir = 4;
	setQueDataWaterAiroutEviTemper(water, airout, envir);
	setQueDataColdModelEvaT(5);

	setInitTestValveData(ValveMainA);
	iQUE_setWorkerModel(SIG_MAKE_COLD);
	ValveCalc_WorkerModelChangeParams();

	ValveCalc_popSig(&tsig);
	//1times Back
	setQueDataInEvaTemper(6,5);
	setInitTestValveData(0);
	setQueDataAirOutT(90);


	ValveCalc_calcValveMain(ValveMainA);
	ValveCalc_popSig(&tsig);

	TEST_ASSERT_EQUAL(VALVE_STEPSInit,tsig.code);

	//2times Back
	ValveCalc_calcValveMain(ValveMainA);
	ValveCalc_popSig(&tsig);

	TEST_ASSERT_EQUAL(VALVE_STEPSInit,tsig.code);

	//3times Back
	ValveCalc_calcValveMain(ValveMainA);
	ValveCalc_popSig(&tsig);

	TEST_ASSERT_EQUAL(VALVE_STEPSInit,tsig.code);

	//4times Back
	ValveCalc_calcValveMain(ValveMainA);
	ValveCalc_popSig(&tsig);

	TEST_ASSERT_EQUAL(VALVE_STEPSInit,tsig.code);

	TEST_ASSERT_EQUAL(0,testValveData[ValveMainA].valveCounts);

	//1times foward
	setInitTestValveData(0);
	setQueDataInEvaTemper(20,13);

	ValveCalc_calcValveMain(ValveMainA);
	ValveCalc_popSig(&tsig);

	TEST_ASSERT_EQUAL(valveRun,tsig.sig);
	TEST_ASSERT_EQUAL(VALVE_STEPSInit*(-1),tsig.code);

	//2times foward
	ValveCalc_calcValveMain(ValveMainA);
	ValveCalc_popSig(&tsig);

	TEST_ASSERT_EQUAL(VALVE_STEPSInit*(-1),tsig.code);

	//3times foward
	ValveCalc_calcValveMain(ValveMainA);
	ValveCalc_popSig(&tsig);

	TEST_ASSERT_EQUAL(VALVE_STEPSInit*(-1),tsig.code);

	//4times foward
	ValveCalc_calcValveMain(ValveMainA);
	ValveCalc_popSig(&tsig);

	TEST_ASSERT_EQUAL(VALVE_STEPSInit*(-1),tsig.code);
}


