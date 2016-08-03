#include "unity_fixture.h"

static void RunAllTests(void)
{
  //RUN_TEST_GROUP(ProductionCode);
  RUN_TEST_GROUP(ADC_simpling);
  RUN_TEST_GROUP(INP_err);
  RUN_TEST_GROUP(BSP_cd4051);
  RUN_TEST_GROUP(User_IODetectErr);
  RUN_TEST_GROUP(INPData);
  RUN_TEST_GROUP(QueueProcess);
  RUN_TEST_GROUP(RTdataStruct);
  RUN_TEST_GROUP(Uart1Dma);
  RUN_TEST_GROUP(ValveCalc);
  RUN_TEST_GROUP(Uart2Dma);
  RUN_TEST_GROUP(Relays);

}

int main(int argc, const char * argv[])
{
  return UnityMain(argc, argv, RunAllTests);
}
