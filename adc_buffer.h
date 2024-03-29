// Implements a thread-safe circular buffer to hold values taken from the ADC.
// Can change size of the buffer at will with resize function.
// The thread-safe-ness is implemented by using coarse-grained synchronization with a mutex.
#ifndef ADC_BUFFER_H_
#define ADC_BUFFER_H_

#include "adc.h"
#include <pthread.h>

#define ADC_BUFFER_PREV_SAMPLE_WEIGHT 0.999

// NOTE: The correct functioning of this data structure depends on not modifying any of these
// properties directly. Use the provided functions to do that instead.
typedef struct {
    adc_in* array;
    uint64 start;
    uint64 end;
    uint64 capacity;
    uint64 size;
    uint64 totalNumSamplesTaken;
    double currentMean;
    pthread_mutex_t mutex;
} AdcBuffer;

// Not thread-safe.
void AdcBuffer_init(AdcBuffer* pBuffer, uint64 n);

// Thread-safe.
uint64 AdcBuffer_getSize(AdcBuffer* pBuffer);
uint64 AdcBuffer_getMaxSize(AdcBuffer* pBuffer);
adc_in* AdcBuffer_getSamples(AdcBuffer* pBuffer, uint64* outNumSamples);
uint64 AdcBuffer_getTotalNumSamplesTaken(AdcBuffer* pBuffer);
double AdcBuffer_getCurrentMean(AdcBuffer* pBuffer);
void AdcBuffer_resize(AdcBuffer* pBuffer, uint64 m);
adc_in AdcBuffer_get(AdcBuffer* pBuffer, uint64 i);
void AdcBuffer_add(AdcBuffer* pBuffer, adc_in x);

// Not thread-safe.
void AdcBuffer_cleanup(AdcBuffer* pBuffer);

// Not thread-safe. For debugging.
void AdcBuffer_printProperties(AdcBuffer* pBuffer);
void AdcBuffer_printArray(AdcBuffer* pBuffer);
void AdcBuffer_print(AdcBuffer* pBuffer);
void AdcBuffer_printAll(AdcBuffer* pBuffer);

#endif