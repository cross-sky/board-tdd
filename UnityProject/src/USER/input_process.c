#include "cominc.h"

#define ADC_DATA_MIN 50		//NTC open
#define ADC_DATA_MAX 4000	// NTC short


InputProcessStruct dataInput={
	0
};

void vInp_init(void)
{
	uint8_t i;
	for(i=0;i<ADCMAX;i++)
	{
		dataInput.usedNtc[i] = ADC_USED;
	}
	dataInput.usedNtc[ADC07_WOUT] = ADC_UNUSED;
}

uint16_t Inp_getFinalCd4051(void)
{
	return dataInput.finaCD4051;
}

void Inp_setFinalCd4051(uint16_t data)
{
	dataInput.finaCD4051 = data;
}

void Inp_setDataCd4051(uint16_t data,uint8_t runCount)
{
	dataInput.dataCd4051[runCount] = data;
}

uint16_t Inp_getDataCd4051(uint8_t runCount)
{
	return  dataInput.dataCd4051[runCount];
}

ptrInputProcessStruct INP_getDataInput(void)
{
	return &dataInput;
}

uint8_t isIdInRange(uint8_t id)
{
	//id>=0 && �޷������κ� 0 ��������Ƚ�
	return (id<ADCMAX);
}

uint8_t isNtcUsed(uint8_t id)
{
	if (isIdInRange(id))
	{
		return dataInput.usedNtc[id];
	}
	else
		return ADC_UNUSED;
}


uint16_t Inp_adcGetErr(void)
{
	uint8_t i;
	uint16_t data;
	ErrStruct err;
	//1.check is there alread have an error,or err type is err_ntc

	ERR_getErr(&err);
	if (err.errUsed == ERR_USED && err.errType!=ERR_NTC)
	{
		return ERR_USED;
	}

	//���2��adc�Ǽ���ڲ��¶ȣ�����(��ʱûд)������Ҫerr
	for (i=0;i<ADCMAX-1;i++)
	{
		if (isNtcUsed(i) == ADC_USED)
		{
			data = dataInput.finaNtc[i];
			if (data <ADC_DATA_MIN || data>ADC_DATA_MAX)
			{
				//an error
				ERR_setErr(ERR_NTC,i);
				return ERR_USED;
			}
		}
	}

	if (err.errUsed == ERR_USED)
	{
		//ntc�ɴ���תΪ����������һ���¼�(��ʾ��ת���ػ�״̬)...
		ERR_clearErr();

		//���͹ػ���ת���ػ�״̬����ʱ��ʾ����µ��ػ�״̬��
		//������ʾ���ж��Ƿ��ٴο���
		xQUESigPush(SIG_OFF);
	}

	return ERR_UNUSED;
}

void Inp_checkTempera110(void)
{
	uint16_t tdata;
	if (dataInput.realNtc[ADC05_AOUT] >= 1100)
	{
		tdata = CDIN_OPEN;
	}
	else
		tdata = 1;

	IOERR_checkErr(ERR_TEMPERA110, tdata);
}

void Inp_adcMeanMethod(void)
{
	uint8_t i,j;
	uint16_t data=0;
	//[5][8]
	for(i=0;i<ADCMAX;i++)
	{
		if (isNtcUsed(i) == ADC_USED)
		{
			for(j=0;j<ADC_NeedRunCount;j++)
			{
				data += dataInput.dataNtc[j][i];
			}
			dataInput.finaNtc[i] = data/ADC_NeedRunCount;
			data = 0;
		}
	}
}


void Inp_convertRealNtc(void)
{
	uint16_t searchValue,index;
	float temp;
	uint8_t i;

	//���2��adc���ⷽ�����㣬�ڲ��¶Ⱥ͵���
	for(i=0;i<ADCMAX-2; i++)
	{
		searchValue=dataInput.finaNtc[i];
		index=uADCSearchData10K(searchValue);
		dataInput.realNtc[i]=iADCTemperCalc10K(index,searchValue);
	}

	{
		searchValue=dataInput.finaNtc[ADC05_AOUT];
		index=uADCSearchData10KV2(searchValue);
		dataInput.realNtc[ADC05_AOUT]=iADCTemperCalc10KV2(index,searchValue);
	}

	temp=(float)dataInput.finaNtc[ADCMAX-2]*0.00080f;
	temp = (float)(1.43-temp)/0.0043f+25;
	dataInput.realNtc[ADCMAX-2]=(uint16_t)(temp*10);

	dataInput.realNtc[ADCMAX-1] = dataInput.finaNtc[ADCMAX-1];

}


void Inp_adcDataProcess(void)
{
	ptrState adcstate;
	//1.��ȡ״̬
	adcstate = BspAdc_getPtrStateAdc();

	//2.�ɼ�����Ƿ��Ѵ���
	if (State_getDataProcessedFlag(adcstate) == STATE_DATA_DONE)
	{
		return;
	}
	//3.��ֵ����
	Inp_adcMeanMethod();
	//4.���Ҵ���
	Inp_adcGetErr();
	//5.ת������
	Inp_convertRealNtc();

	//5.1 ��������¶ȴ���110��
	Inp_checkTempera110();
	//6.��ɱ�־
	State_setDataProcessedFlag(adcstate,STATE_DATA_DONE);

	//7.ת�������õ��ڴ����
	vInpSetCoreParamADC(&dataInput);
}


uint16_t Inp_getFinalCd4051Inx(uint16_t cdInX)
{
	return (dataInput.finaCD4051 >> cdInX)&0x01;
	//0 means err
	//1.get high Pressure err
	//2.get low Pressure error
	//3.get water open err

	//�ߵ�ѹ������������3min�ڼ��Ͽ���ͣ��1�Σ��ָ��������Զ�����������10min��ͣ��3�Σ�������󣬹ػ�������������
	//ˮ��������10s�ڼ��Ͽ���ͣ��1�Σ��ָ��������Զ�����������ͣ��3�Σ�������󣬹ػ�������������

}

void Inp_cd4051Method(void)
{
	uint8_t i,j;
	uint16_t data=0;
	uint16_t finalData=0;
	//[5]
	for(i=0;i<CDinMax;i++)
	{
		for (j=0;j<ADC_NeedRunCount;j++)
		{
			if ((dataInput.dataCd4051[j]>>i)&0x01)
			{
				data++;
			}
		}
		if (data >= ADC_NeedRunCount-1)
		{
			data=1;
		}
		else
			data=0;
		finalData |= data<<i;
		data = 0;

	}
	dataInput.finaCD4051 = finalData;
}


void Inp_cd4051DataProcess(void)
{
	ptrState cd4051state;
	//1.��ȡ״̬
	cd4051state = BspAdc_getPtrStateCd4051();

	//2.�ɼ�����Ƿ��Ѵ���
	if (State_getDataProcessedFlag(cd4051state) == STATE_DATA_DONE)
	{
		return;
	}
	//3.��ֵ����
	Inp_cd4051Method();
	//4.���Ҵ���
	IOERR_checkErr(ERR_HIGH_PREESURE, Inp_getFinalCd4051Inx(CDin01_WATER_OPEN));
	IOERR_checkErr(ERR_LOW_PREESURE, Inp_getFinalCd4051Inx(CDin02_LOW_PREESURE));

	if (IODECT_getFlagCheckWaterOpen() == STATE_ON)
	{
		IOERR_checkErr(ERR_WATER_OPEN, Inp_getFinalCd4051Inx(CDin03_HIGH_PREESURE));
	}
	//err detect

	//5.��ɱ�־
	State_setDataProcessedFlag(cd4051state,STATE_DATA_DONE);
	//ת�������õ��ڴ����
	vInpSetCoreParamCd4051(dataInput.finaCD4051);
}

uint16_t* uINPGetADCaddress(StateStruct* ptrState)
{
	//dataInput.countNtc++;
	//t=data[i++]; ����ȡdata[i]��ֵ��i++

	return &(dataInput.dataNtc[ptrState->runCount][0]);
}


void Task1RegularSimpling(void)
{
	vADCSimpling();
	vCd4051Simpling();
}

void Task2InputProcess(void)
{
	Inp_adcDataProcess();
	Inp_cd4051DataProcess();
	vInpSetErrState();
}



