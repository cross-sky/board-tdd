#ifndef __IODETECTERR_H
#define __IODETECTERR_H

#include <stdint.h>

typedef struct IODectectErrStruct{
	uint16_t errDetectType;	//检测目标错误类型
	uint16_t state;			//检测到错误的开始标志
	uint16_t stopTimes;		//连续停机次数
	uint16_t runCount;		//检测次数
	uint16_t MaxRunCount;	//最大检测次数
	uint16_t errCount;		//检测到错误的数据次数
}IODectectErrStruct,*ptrIODectectErr;

typedef enum IodectectErrType{
	IOERR_High_PRESSURE,
	IOERR_LOW_PRESSURE,
	IOERR_WATER_OPEN,
	IOERR_TEMPERA110,
	IOERR_MAX,
	IOERR_OFF,
	IOERR_ON
}IodectectErrType;


void IOERR_checkErr(ErrType errType, uint16_t inXData);
ptrIODectectErr IODect_getStructAddress(void);
void IODECT_setIoDetectData(ptrIODectectErr dstErrAdrees, ptrIODectectErr srcErrAdrees);
void IODECT_startCheckWaterOpen(void);
void IODECT_stopCheckWaterOpen(void);
uint8_t IODECT_getFlagCheckWaterOpen(void);

#endif

