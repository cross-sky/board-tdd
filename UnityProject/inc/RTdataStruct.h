#ifndef __RTDATASTRUCT_H
#define __RTDATASTRUCT_H

#define RTxBUF_MAX_COUNTER 5
#define RXBUF_SIZE 39
#define TXBUF_SIZE 79

//���������������ˣ�Ų������
typedef struct{
	uint8_t machineState;	//��/�ػ�״̬
	uint8_t elecState;	//����ȴ�/�ر�״̬
}RunState_t;

typedef struct{
	uint16_t state;				//״̬�����
	int16_t inTemper;			//�����¶�
	int16_t outTemper;			//�����¶�
	int16_t evaporateTemper;	//�����¶�
	int16_t current;			//����
	int16_t valveMainStep;		//���������ͷ�����
	int16_t valveSubStep;		//�����������ͷ�����
}Machine_t;

typedef struct DataFrameStruct 
{
	uint8_t length;
	uint8_t command;
}DataFrameStruct;


typedef struct Command1RequestDataStruct
{
	DataFrameStruct dataFrame;
	uint8_t onOrOff;
	uint8_t elect;
	uint8_t defrost;
}Command1RequestDataStruct;

#define FrameStart1Code 0xfe
#define FrameStart2Code 0xef
#define FrameEndCode	0xca

typedef enum FUNCODE{
	FunCode1=0xf1,
	FunCode2,
	FunCode3,
	FunCode4,
	FunCode5,
	FunCode6,
	FunCodeMax
}FUNCODE;


//��0λ��ʼ
#define RTX_START_BIT	0

typedef struct{
	uint8_t length;
	uint8_t buff[RXBUF_SIZE];
}UartNodeRx_t;

typedef struct RTdataNode {
	uint8_t length;
	uint8_t *buff;
}RTdataNode,*ptrRTdataNode;

typedef struct UartProcess_t{
	uint8_t in;
	uint8_t out;
	uint8_t max;
	UartNodeRx_t node[RTxBUF_MAX_COUNTER];
}UartProcess_t,*ptrUartNodesProcess;


typedef struct Command12ReturnDataStruct{
	DataFrameStruct dataFrame;
}Command12ReturnDataStruct;

typedef struct Command2RequestDataStruct{
	DataFrameStruct dataFrame;
	int8_t setWaterT;
	int8_t setWaterCycleTemper;
	int8_t setDefrostInTemper;
	int8_t setDefrostRunTimes;
	int8_t setDefrostOutTemper;
	int8_t setDefrostCycleTimes;
	int8_t setDefrostInOffset;
	int8_t setAirOut_WaterT;	//����-ˮ�²�
}Command2RequestDataStruct;

//command2returnDataStruct is the same as Command1ReturnDataStruct;
//just use the same struct

//return Command3ReturnDataStruct
typedef struct Command3RequestDataStruct{
	DataFrameStruct dataFrame;
}Command3RequestDataStruct;

typedef struct Command3ReturnDataStruct{
	DataFrameStruct dataFrame;
	uint16_t waterSet;
	int16_t waterIn;
	int16_t waterOut;
	int16_t waterBank;
	int16_t evironT;
	int16_t innerTemper;

	uint16_t errType;
	uint16_t cd4051DectState;	//cd4051״̬
	uint16_t relaysValue;
	RunState_t runState;
	Machine_t machineA;
	
}Command3ReturnDataStruct;

//not need to return ok
typedef struct Command4RequestDataStruct{
	DataFrameStruct dataFrame;
	uint8_t outKind;	//�������ѡ���ֽ���(����3��������)�����ַ���(����4��������)�������ڴ���2
}Command4RequestDataStruct;

//return Command12ReturnDataStruct
typedef struct Command5RequestDataStruct{
	DataFrameStruct dataFrame;
	uint16_t data1Relay;	//ָ���̵������
	int8_t data2ValveA;		//ָ��ValveA�������
	int8_t data3ValveB;		//ָ��valveB�������
	int16_t workModel;		//ָ������ģʽ
}Command5RequestDataStruct;

uint8_t* puartGetRTxAddress(ptrUartNodesProcess nodes);

uint8_t RT_uartRxPush(uint8_t len,ptrUartNodesProcess nodes);
uint8_t RT_checkFrameStart(ptrRTdataNode ptrNode);
uint8_t RT_checkFrameEnd(ptrRTdataNode ptrNode);

uint8_t RT_dataAdd(uint8_t len, uint8_t *addr);

uint8_t RT_getNodesCount(ptrUartNodesProcess nodes);
void RT_addOutIndex(ptrUartNodesProcess nodes);


void RT_command1Receiverequest(Command1RequestDataStruct *ptrC1);
void RT_command2ReceiveRequest(Command2RequestDataStruct *ptrC2);
uint8_t RT_command4ReceiveRequest(Command4RequestDataStruct *ptrc4);
void RT_command5ReceiveRequest(Command5RequestDataStruct *ptrc5);

void RT_command3CreateRequest(Command3RequestDataStruct *ptrc3);
void RT_command2CreateRequest(Command2RequestDataStruct *ptrc2);
void RT_command1CreateRequest(Command1RequestDataStruct *ptrc1);
void RT_command4CreateRequest(Command4RequestDataStruct *ptrc4);
void RT_command5CreateRequest(Command5RequestDataStruct *ptrc5, Command5RequestDataStruct *ptrc5Data);


uint8_t RT_command12SendReturn(uint8_t funcode,uint8_t *txAddr);
uint8_t RT_command3SendReturn(uint8_t funcode,uint8_t *txAddr);
uint16_t RT_command4SendReturn(int8_t *txAddr);

#endif

