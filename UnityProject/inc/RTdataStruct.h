#ifndef __RTDATASTRUCT_H
#define __RTDATASTRUCT_H

#define RTxBUF_MAX_COUNTER 5
#define RXBUF_SIZE 39
#define TXBUF_SIZE 79

//这两个交叉引用了，挪到这里
typedef struct{
	uint8_t machineState;	//开/关机状态
	uint8_t elecState;	//电加热打开/关闭状态
}RunState_t;

typedef struct{
	uint16_t state;				//状态开或关
	int16_t inTemper;			//吸气温度
	int16_t outTemper;			//排气温度
	int16_t evaporateTemper;	//蒸发温度
	int16_t current;			//电流
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
	FunCodeMax
}FUNCODE;


//第0位开始
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
}Command2RequestDataStruct;

//command2returnDataStruct is the same as Command1ReturnDataStruct;
//just use the same struct

//Command3RequestDataStruct 

typedef struct Command3RequestDataStruct{
	DataFrameStruct dataFrame;
}Command3RequestDataStruct;

typedef struct Command3ReturnDataStruct{
	DataFrameStruct dataFrame;
	int16_t waterIn;
	int16_t waterOut;
	int16_t waterBank;
	int16_t evironT;
	int16_t innerTemper;
	uint16_t valvesteps;

	RunState_t runState;
	Machine_t machineA;
}Command3ReturnDataStruct;

uint8_t RT_uartRxPush(uint8_t len,ptrUartNodesProcess nodes);
uint8_t RT_checkFrameStart(ptrRTdataNode ptrNode);
uint8_t RT_checkFrameEnd(ptrRTdataNode ptrNode);

void RT_command1Receiverequest(Command1RequestDataStruct *ptrC1);

uint8_t RT_dataAdd(uint8_t len, uint8_t *addr);
uint8_t RT_command12SendReturn(uint8_t funcode,uint8_t *txAddr);
void RT_command2ReceiveRequest(Command2RequestDataStruct *ptrC2);
uint8_t RT_command3SendReturn(uint8_t funcode,uint8_t *txAddr);

void RT_command3CreateRequest(Command3RequestDataStruct *ptrc3);
void RT_command2CreateRequest(Command2RequestDataStruct *ptrc2);
void RT_command1CreateRequest(Command1RequestDataStruct *ptrc1);

//void RT_Command1Process(UartNodeRx_t *node);
//void RT_Command2Process(UartNodeRx_t *node);
//void RT_Command3Process(UartNodeRx_t *node);

//ptrFunComand RT_getFunFromCommandCode(FUNCODE funCode);

#endif

