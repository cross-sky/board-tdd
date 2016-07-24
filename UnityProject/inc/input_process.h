#ifndef __INPUT_PROCESS_H
#define __INPUT_PROCESS_H


typedef struct InputProcessStruct{
	uint8_t usedNtc[ADC_MaxNUmbers];
	uint16_t dataNtc[ADC_NeedRunCount][ADC_MaxNUmbers];
	uint16_t finaNtc[ADC_MaxNUmbers];
	int16_t realNtc[ADC_MaxNUmbers];
	uint16_t dataCd4051[ADC_NeedRunCount];
	uint16_t finaCD4051;
}InputProcessStruct, *ptrInputProcessStruct;

ptrInputProcessStruct INP_getDataInput(void);
uint16_t Inp_adcGetErr(void);
void Inp_setDataCd4051(uint16_t data,uint8_t runCount);
void Inp_adcDataProcess(void);
void Inp_cd4051DataProcess(void);
uint16_t Inp_getDataCd4051(uint8_t runCount);
uint32_t* uINPGetADCaddress(StateStruct* ptrState);
/*
#define INPUT_COUNT_MAX 5
#define INPUT_TIMES_MAX 20
typedef struct{
	uint8_t countCd4051;
	uint8_t countNtc;
	uint16_t dataCd4051[INPUT_COUNT_MAX];

	uint16_t finaCD4051;
}InputProcess_t;

extern InputProcess_t dataInput;

uint8_t uINPCd405xExcuted(void);
void vINPSendCd405xData(uint16_t data);
uint16_t* uINPGetADCaddress(void);
uint8_t uINPADCExcuted(void);
void vINPADCCompleteSampling(void);

void Task2InputProcess(void);
void Task1RegularSimpling(void);
*/

#endif
