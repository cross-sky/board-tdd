#ifndef __INPUT_PROCESS_H
#define __INPUT_PROCESS_H


typedef struct InputProcessStruct{
	uint8_t usedNtc[ADCMAX];
	uint16_t dataNtc[ADC_NeedRunCount][ADCMAX];
	uint16_t finaNtc[ADCMAX];
	int16_t realNtc[ADCMAX];
	uint16_t dataCd4051[ADC_NeedRunCount];
	uint16_t finaCD4051;
}InputProcessStruct, *ptrInputProcessStruct;

ptrInputProcessStruct INP_getDataInput(void);
uint16_t Inp_adcGetErr(void);
void Inp_setDataCd4051(uint16_t data,uint8_t runCount);
void Inp_adcDataProcess(void);
void Inp_cd4051DataProcess(void);
uint16_t Inp_getDataCd4051(uint8_t runCount);
uint16_t* uINPGetADCaddress(StateStruct* ptrState);

void Task1RegularSimpling(void);
void Task2InputProcess(void);


#endif
