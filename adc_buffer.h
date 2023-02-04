// Implements a circular buffer to hold ADC values. Can change size at will.
#ifndef ADC_BUFFER_H_
#define ADC_BUFFER_H_

#include "adc.h"

typedef struct {
    adc_in* array;
    uint64 start;
    uint64 end;
    uint64 capacity;
    uint64 size;
} AdcBuffer;

void AdcBuffer_init(AdcBuffer* pBuffer, uint64 n);
void AdcBuffer_resize(AdcBuffer* pBuffer, uint64 m);
adc_in AdcBuffer_get(AdcBuffer* pBuffer, uint64 i);
void AdcBuffer_add(AdcBuffer* pBuffer, adc_in x);
void AdcBuffer_cleanup(AdcBuffer* pBuffer);
void AdcBuffer_printProperties(AdcBuffer* pBuffer);
void AdcBuffer_printArray(AdcBuffer* pBuffer);
void AdcBuffer_print(AdcBuffer* pBuffer);
void AdcBuffer_printAll(AdcBuffer* pBuffer);

#endif