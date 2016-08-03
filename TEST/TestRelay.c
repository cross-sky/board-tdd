#include "cominc.h"
#include "unity.h"
#include "unity_fixture.h"

static RelayAndValveDataStruct* tdataRelay;

static void setRelayData(uint16_t relayValveA, uint16_t valveB, uint16_t backValue)
{
	tdataRelay->relaysAndValveMainA = relayValveA;
	tdataRelay->valveSubB = valveB;
	tdataRelay->tValveValue[0] = backValue;
	tdataRelay->tValveValue[1] = backValue;
}


TEST_GROUP(Relays);

TEST_SETUP(Relays)
{
	tdataRelay = vRelay_getDataStruct();
}

TEST_TEAR_DOWN(Relays)
{

}

TEST(Relays, backValveValue)
{
	uint16_t relayValveA= 0xffff;
	uint16_t valveB = 0xf7ff;

	setRelayData(relayValveA,valveB,0);

	vRelay_backValveValue(ValveMainA);
	TEST_ASSERT_EQUAL(0xe1ff,tdataRelay->relaysAndValveMainA);

	vRelay_backValveValue(ValveSubB);
	TEST_ASSERT_EQUAL(0x07ff,tdataRelay->valveSubB);
}

