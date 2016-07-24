#ifndef _ERRTYPE_H
#define _ERRTYPE_H

#include <stdint.h>

typedef enum ErrType{
	ERR_UNUSED,
	ERR_USED,
	ERR_NTC,
	ERR_LOW_PREESURE,
	ERR_HIGH_PREESURE,
	ERR_WATER_OPEN,
	ERR_TEMPERA110,
	ERR_DISCONNECT,
}ErrType;

typedef struct ErrStruct{
	uint16_t errType;
	uint16_t id;
	uint16_t errUsed;
}ErrStruct, *ptrErr;


uint16_t ERR_usedFlag(void);
void ERR_setErr(uint16_t ErrType, uint16_t id);
void ERR_getErr(ptrErr e);
void ERR_clearErr(void);

#endif

