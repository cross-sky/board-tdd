#ifndef __VALVE_CALC_H
#define __VALVE_CALC_H

#define ValveBuffLen 16
#define ValveBuffLenMask 0x0f

#define VALVE_INIT_STEP -560
#define VALVE_CLOSE_STEP 8
#define VALVE_MIN_STEP	30
#define VALVE_MAX_STEP	470
#define VALVE_COLDMAX_STEP 120
#define VALVE_INITRUN_STEP		100

typedef enum{
	adds=2,
	decr,
	statusRun,
	statusDone,
	valveInit,
	valveClose,
	valveRun,
	DirectBack,		//������(����)
	DirectHold,		//������
	DirectForward,	//���з��� 1������(����)
}VALVE_ENUM;

typedef enum{
	IndexDirectBack,
	IndexDirectHold,
	IndexDirectForward,
	IndexDirectMax
}IndexValveDirect_Enum;

typedef enum ValveKinds{
	ValveMainA = 0x00,
	ValveSubB,
	ValveKindsMax
}ValveKinds;

typedef struct{
	int16_t code;// ���в���
	uint16_t sig;// ��ʼ/��˪/����״̬
	uint16_t kindValue; //���������ͷ����Ǹ��������ͷ�
}ValveSig_t;

typedef struct 
{
	uint8_t in;	//�������ӳ��ȣ���������
	uint8_t out;
	ValveSig_t buff[ValveBuffLen];
}ValveProcess_t;

typedef struct{
	uint16_t valveStatus;	//�����Ƿ�����
	int16_t totalSteps;		//��ǰ����
	int16_t runStep;	//��Ҫ���в���
	int16_t lastStep;//���һ�β���
	int16_t valveDirection;//���з���direct**
	int16_t valveCounts;//�������ӻ���ٴ���
}ValveStatus_t;

void ValveCalv_changeValveData(ValveStatus_t* src);
uint8_t ValveCalc_popSig(ValveSig_t *sig);
uint8_t ValveCalc_pushSig(ValveSig_t *srcSig);
ValveProcess_t* ValveCalc_getSigNodes(void);
void checkTotalSteps(ValveSig_t *sig);
void ValveCalc_checkProcess(ValveKinds valveKind);
void ValveCalc_valveRun(ValveKinds valveKind);
void ValveCalc_calcValveMain(ValveKinds valveKind);

void ValveCalc_command5PushSig(int8_t data, uint16_t valveKind);

void vTask_valveProcess(void);
void ValveCalc_setStartFlag(uint8_t state);


void vTask_valveCalc(void);

void ValveCalc_valveInit(void);
void ValveCalc_valveClose(ValveKinds valveKind);

void ValveCalc_calcValveSub(ValveKinds valveKind);
void ValveCalc_WorkerModelChangeParams(void);
void ValveCalc_defrostValveSet(void);
void ValveCalc_stepsSetTo(ValveKinds valveKind, int16_t steps);

#endif
