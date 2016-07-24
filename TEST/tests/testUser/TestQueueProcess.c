#include "cominc.h"
#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP(QueueProcess);
TEST_SETUP(QueueProcess)
{
	vQUEInit();
}

TEST_TEAR_DOWN(QueueProcess)
{

}

TEST(QueueProcess, TestDelay3Min)
{
	uint16_t i;
	TEST_ASSERT_EQUAL(0,vQueCheck3MinDelay());

	for (i=0;i<=20*180;i++)
	{
		vQueCheck3MinDelay();
	}
	TEST_ASSERT_EQUAL(1,vQueCheck3MinDelay());
}

TEST(QueueProcess, OffStateToOnstateThenToOffState)
{
	TEST_ASSERT_EQUAL(SIG_OFF,vqueGetMachineState());
	xQUESigPush(SIG_ON);
	vTask3QUEProcess();
	TEST_ASSERT_EQUAL(SIG_ON,vqueGetMachineState());
	xQUESigPush(SIG_OFF);
	vTask3QUEProcess();
	TEST_ASSERT_EQUAL(SIG_OFF,vqueGetMachineState());
}