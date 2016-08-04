#ifndef __QUEUE_PROCESS_H
#define __QUEUE_PROCESS_H

//#include "RTdataStruct.h"

#define QueueBuffLen 32
#define QueueBuffLenMask 0x1f

typedef uint8_t (*ptrfuntion)(void);

typedef struct{
	//	uint8_t code;
	uint8_t sig;
}QueueSig_t;

typedef struct 
{
	uint8_t in;
	uint8_t out;
	QueueSig_t buff[QueueBuffLen];
}QueueProcess_t;


typedef enum SigState{
	SIG_NULL=0x00,		//
	SIG_ON,				//
	SIG_DEFROST,		//��˪
	SIG_HOLD,			//����
	SIG_LOWT,			//������ <-15
	SIG_OFF,			//�ػ�״̬
	SIG_ERR,			//����
	SIG_NOERR,			//�޴���
	SIG_MAX,
	FUN_EXCUTED,		//״̬ת�����
	FUN_STATE_INIT,		//������ʼ��״̬
	FUN_STATE_RUN,		//��������״̬
	FUN_STATE_EXIT		//�����˳�״̬
}SigState;


typedef struct{
	int16_t SetWaterTSet;		//����ˮ��
	int16_t setWaterCycleTemper;	//����ˮ�»ز�
	int16_t setDefrostInTemper;	//�����˪�¶�
	uint32_t setDefrostRunTimes;		//��˪ʱ��

	int16_t setDefrostOutTemper;		//�˳���˪�¶�
	uint32_t setDefrostCycleTimes;		//��˪����
	int16_t setDefrostInOffset;			//��˪����ƫ��

	int16_t setDefrostMode;			//��˪���Ʒ�ʽ	

	int16_t setValveMode;			//�������ͷ����Ʒ�ʽ
	int16_t setValveManulSteps;		//�ֶ��������ͷ�����
	int16_t setSuperHeatLow5;		//����5�����¹��ȶ�
	int16_t setSuperHeatOver5;		//����5�����Ϲ��ȶ�
	int16_t setElecHeatOnTemper;	//���ȹ����������趨�¶�

	int16_t setElecOnState;			//ǿ�����������
	int16_t setCompressorNumbers;	//ѹ��������
	int16_t setCurrentProtectMax;	//����������׼
	int16_t setPhaseProtect;		//��λ����
	//��������..
	uint16_t errState;	//err����״̬
	uint16_t cd4051DectState;	//cd4051����״̬

	int16_t waterIn;
	int16_t waterOut;
	int16_t waterBank;			//ˮ���¶�
	int16_t environT;			//����
	int16_t innerTemper;		//�ڲ��¶�
	uint16_t valvesteps;
	Machine_t machineA;
}CoreParams_t;

typedef struct{
	uint8_t funExcuted;			//״̬�Ƿ�ת�����
	uint8_t funState;			//״̬
	ptrfuntion tempfun;
	ptrfuntion functions;
	ptrfuntion prefunction;
	RunState_t runState;
	CoreParams_t coreParems;
}CoreProcess_t;

void vQueSetCoreParams(Command2RequestDataStruct *srcData);

void vInpSetCoreParamADC(ptrInputProcessStruct srcData);

uint8_t xQUESigPush(uint8_t sig);
uint8_t vqueGetMachineState(void);
void vTask3QUEProcess(void);
uint8_t vQueCheck3MinDelay(void);
void vInpSetCoreParamCd4051(uint16_t srcData);

void vInpSetErrState(void);

void vQueSetCoreParamErrstate(uint16_t errType);

void vQUEGetTemperParams(Command3ReturnDataStruct *dstData);

int16_t iQUE_getInTemper(void);
int16_t iQUE_getEvaporateTemper(void);
int16_t iQUE_getSuperheat(void);

CoreProcess_t* xQue_getCoreData(void);

void vTaskQUEProcess(void);
void vQUEInit(void);

void iQUE_ValveChanges(ValveKinds valveKind,uint16_t step);
/*
void vQUESetADCDectState(uint16_t adcstate);
*/
#endif

