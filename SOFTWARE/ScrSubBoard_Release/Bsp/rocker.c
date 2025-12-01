#include "rocker.h"
#include "adc.h"

#define MIDDLE_DATA_MAX            10
typedef struct
{
	float data[MIDDLE_DATA_MAX];
	float result;
}middleFilter_t;

middleFilter_t rocker[4] = {0};

uint16_t adcDataRaw[4] __attribute__((aligned(4))) = {0};
uint16_t adcDataRes[4] __attribute__((aligned(4))) = {0};

static float middleFilter_Cal(middleFilter_t* fil, float in_data)
{
	float sum = 0;
	float temp[MIDDLE_DATA_MAX];
	float change;
	uint8_t i = 0,j = 0;
	for(i=0; i<MIDDLE_DATA_MAX - 1; i++)
	{
		fil->data[i]=fil->data[i+1];
	}
	fil->data[MIDDLE_DATA_MAX - 1] = in_data;
	for(i=0; i<MIDDLE_DATA_MAX; i++)          
		temp[i] = fil->data[i];
	for(i=1; i<MIDDLE_DATA_MAX; i++)
		for(j=0; j<MIDDLE_DATA_MAX-i; j++)
		{
			if(temp[j] > temp[j+1])
			{
				change = temp[j];
				temp[j] = temp[j+1];
				temp[j+1] = change;
			}
		}
	for(i=1; i<MIDDLE_DATA_MAX-1; i++)
		sum = sum + temp[i];
		
  fil->result = sum/(MIDDLE_DATA_MAX - 2);
	return(fil->result);
}

void rocker_run_Handle(void)
{
	HAL_ADC_Start_DMA(&hadc, (uint32_t*)adcDataRaw, 4);
}

void rocker_convert_finish_Hook(void)
{
	for(uint8_t i = 0; i < 4; i++)
	{
		adcDataRes[i] = middleFilter_Cal(&rocker[i], adcDataRaw[i]);
	}
}

const uint16_t* rocker_get_res_buf(void)
{
	return (uint16_t*)adcDataRes;
}




