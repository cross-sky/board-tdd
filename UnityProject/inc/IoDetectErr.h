#ifndef __IODETECTERR_H
#define __IODETECTERR_H

#include <stdint.h>

typedef struct IODectectErrStruct{
	uint16_t errDetectType;	//���Ŀ���������
	uint16_t state;			//��⵽����Ŀ�ʼ��־
	uint16_t stopTimes;		//����ͣ������
	uint16_t runCount;		//������
	uint16_t MaxRunCount;	//��������
	uint16_t errCount;		//��⵽��������ݴ���
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

