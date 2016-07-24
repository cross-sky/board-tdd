#ifndef __STM32_UB_RELAY_H
#define __STM32_UB_RELAY_H

typedef enum{
	Relay01Compressor=0x00,
	Relay02Valve4way,
	Relay03Pump,
	Relay04ElecHeater,
	Relay05,
	Relay06,
	Relay07,
	Relay08,
	Relay09,
	Relay10Motor,
	Relay11,
	RelayMax
}RelayName_t;

typedef struct RelayAndValveDataStruct{
	uint16_t relaysAndValveMainA;	//relay µÍÎ»£¬ valveA ¸ßÎ»(12-15)
	uint16_t valveSubB;
}RelayAndValveDataStruct;

void vRelaySet(RelayName_t relay, StateType state);
void HardValve_runDirect(uint16_t addsOrDecr,uint16_t valveKind);

/*
void vRelaySetAll(void);
void vRelayClearAll(void);
void vTask4RelayOutProcess(void);
void vRelayInit(void);

*/
#endif

