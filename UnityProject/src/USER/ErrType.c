#include "cominc.h"

static ErrStruct err={0,0,0};

uint16_t ERR_usedFlag(void)
{
	return (uint16_t)err.errUsed;
}

void ERR_setErr(uint16_t ErrType, uint16_t id)
{
	err.errType = ErrType;
	err.id = id;
	err.errUsed = ERR_USED;
}

void ERR_getErr(ptrErr e)
{
	e->errType = err.errType;
	e->id = err.id;
	e->errUsed = err.errUsed;
}

void ERR_clearErr(void)
{
	err.errType = ERR_UNUSED;
	err.id = ERR_UNUSED;
	err.errUsed = ERR_UNUSED;
}

