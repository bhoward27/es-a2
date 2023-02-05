#include <assert.h>
#include <stddef.h>
#include <stdlib.h>

#include "adc_buffer.h"
#include "log.h"

// Internal functions -- not thread safe.
static void _AdcBuffer_verify(AdcBuffer* pBuffer);
static void _AdcBuffer_resize(AdcBuffer* pBuffer, uint64 m);
static adc_in _AdcBuffer_get(AdcBuffer* pBuffer, uint64 i);
static void _AdcBuffer_add(AdcBuffer* pBuffer, adc_in x);
static void _AdcBuffer_updateCurrentMean(AdcBuffer* pBuffer);
static void _AdcBuffer_init(AdcBuffer* pBuffer, uint64 n);

// This function is not thread safe, as it creates the buffer.
void AdcBuffer_init(AdcBuffer* pBuffer, uint64 n)
{
    _AdcBuffer_init(pBuffer, n);
    pthread_mutex_init(&pBuffer->mutex, NULL); // = PTHREAD_MUTEX_INITIALIZER gave me compilation error somehow.
}

static void _AdcBuffer_init(AdcBuffer* pBuffer, uint64 n)
{
    assert(pBuffer);
    assert(n > 0);

    pBuffer->capacity = n + 1;
    pBuffer->size = 0;
    pBuffer->start = pBuffer->end = 0;
    pBuffer->array = malloc(sizeof(pBuffer->array[0]) * pBuffer->capacity);
    if (!pBuffer->array) {
        SYS_DIE("malloc failed!\n");
    }
    pBuffer->totalNumSamplesTaken = 0;
    pBuffer->currentMean = 0.0;
}

uint64 AdcBuffer_getSize(AdcBuffer* pBuffer)
{
    uint64 size = 0;

    int lock_res = pthread_mutex_lock(&pBuffer->mutex);
    {
        if (lock_res != 0) {
            SYS_DIE("pthread_mutex_lock failed with rc = %d.\n", lock_res);
        }

        _AdcBuffer_verify(pBuffer);
        size = pBuffer->size;
    }
    int unlock_res = pthread_mutex_unlock(&pBuffer->mutex);
    if (unlock_res != 0) {
        SYS_DIE("pthread_mutex_unlock failed with rc = %d.\n", unlock_res);
    }

    return size;
}

uint64 AdcBuffer_getTotalNumSamplesTaken(AdcBuffer* pBuffer)
{
    uint64 totalNumSamplesTaken = 0;
    int lock_res = pthread_mutex_lock(&pBuffer->mutex);
    {
        if (lock_res != 0) {
            SYS_DIE("pthread_mutex_lock failed with rc = %d.\n", lock_res);
        }
        _AdcBuffer_verify(pBuffer);
        totalNumSamplesTaken = pBuffer->totalNumSamplesTaken;
    }
    int unlock_res = pthread_mutex_unlock(&pBuffer->mutex);
    if (unlock_res != 0) {
        SYS_DIE("pthread_mutex_unlock failed with rc = %d.\n", unlock_res);
    }

    return totalNumSamplesTaken;
}

adc_in* AdcBuffer_getSamples(AdcBuffer* pBuffer, uint64* outNumSamples)
{
    adc_in* samples = NULL;
    int lock_res = pthread_mutex_lock(&pBuffer->mutex);
    {
        if (lock_res != 0) {
            SYS_DIE("pthread_mutex_lock failed with rc = %d.\n", lock_res);
        }

        _AdcBuffer_verify(pBuffer);
        assert(outNumSamples);

        *outNumSamples = pBuffer->size;
        samples = malloc(sizeof(pBuffer->array[0]) * *outNumSamples);
        if (!samples) {
            SYS_DIE("malloc failed!\n");
        }

        for (uint64 i = 0; i < *outNumSamples; i++) {
            samples[i] = _AdcBuffer_get(pBuffer, i);
        }
    }
    int unlock_res = pthread_mutex_unlock(&pBuffer->mutex);
    if (unlock_res != 0) {
        SYS_DIE("pthread_mutex_unlock failed with rc = %d.\n", unlock_res);
    }

    return samples;
}

// Note: resize actaully changes the capacity of the buffer, not the size.
// Size refers to how many samples are in the array (not how many elements / capacity).
void AdcBuffer_resize(AdcBuffer* pBuffer, uint64 m)
{
    int lock_res = pthread_mutex_lock(&pBuffer->mutex);
    {
        if (lock_res != 0) {
            SYS_DIE("pthread_mutex_lock failed with rc = %d.\n", lock_res);
        }
        _AdcBuffer_resize(pBuffer, m);
    }
    int unlock_res = pthread_mutex_unlock(&pBuffer->mutex);
    if (unlock_res != 0) {
        SYS_DIE("pthread_mutex_unlock failed with rc = %d.\n", unlock_res);
    }
}

// The 'size' in resize is moreso referring to AdcBuffer.capacity
static void _AdcBuffer_resize(AdcBuffer* pBuffer, uint64 m)
{
    _AdcBuffer_verify(pBuffer);
    assert(m > 0);

    uint64 newCapacity = m + 1;

    if (newCapacity == pBuffer->capacity) {
        return;
    }

    AdcBuffer newBuffer;
    _AdcBuffer_init(&newBuffer, m);
    newBuffer.totalNumSamplesTaken = pBuffer->totalNumSamplesTaken;
    newBuffer.currentMean = pBuffer->currentMean;

    // Copy as many of the newest samples as we can fit into newBuffer.
    // NOTE: Could use memcpy instead for some more efficient yet uglier code.
    uint64 newMaxSize = newCapacity - 1;
    uint64 numSamplesToShed = (newMaxSize < pBuffer->size) ? pBuffer->size - newMaxSize : 0;
    for (uint64 i = numSamplesToShed; i < pBuffer->size; i++) {
        _AdcBuffer_add(&newBuffer, _AdcBuffer_get(pBuffer, i));
    }
    pthread_mutex_t temp = pBuffer->mutex;
    free(pBuffer->array);
    *pBuffer = newBuffer;
    pBuffer->mutex = temp;
}

adc_in AdcBuffer_get(AdcBuffer* pBuffer, uint64 i)
{

    adc_in res;

    int lock_res = pthread_mutex_lock(&pBuffer->mutex);
    {
        if (lock_res != 0) {
            SYS_DIE("pthread_mutex_lock failed with rc = %d.\n", lock_res);
        }
       res = _AdcBuffer_get(pBuffer, i);
    }
    int unlock_res = pthread_mutex_unlock(&pBuffer->mutex);
    if (unlock_res != 0) {
        SYS_DIE("pthread_mutex_unlock failed with rc = %d.\n", unlock_res);
    }

    return res;
}

static adc_in _AdcBuffer_get(AdcBuffer* pBuffer, uint64 i)
{
    _AdcBuffer_verify(pBuffer);

    if (pBuffer->size == 0) {
        SYS_DIE("Called %s(%p, %llu), but buffer is empty.\n", __func__, pBuffer, i);
    }
    if (i >= pBuffer->size) {
        SYS_DIE("Index %llu is outside the buffer, which has size %llu.\n", i, pBuffer->size);
    }

    return pBuffer->array[(pBuffer->start + i) % pBuffer->capacity];
}

void AdcBuffer_add(AdcBuffer* pBuffer, adc_in x)
{
    int lock_res = pthread_mutex_lock(&pBuffer->mutex);
    {
        if (lock_res != 0) {
            SYS_DIE("pthread_mutex_lock failed with rc = %d.\n", lock_res);
        }
        _AdcBuffer_add(pBuffer, x);
    }
    int unlock_res = pthread_mutex_unlock(&pBuffer->mutex);
    if (unlock_res != 0) {
        SYS_DIE("pthread_mutex_unlock failed with rc = %d.\n", unlock_res);
    }
}

static void _AdcBuffer_add(AdcBuffer* pBuffer, adc_in x)
{
    assert(x >= ADC_MIN_IN && x <= ADC_MAX_IN);
    _AdcBuffer_verify(pBuffer);

    // Overwrite whatever was here.
    pBuffer->array[pBuffer->end] = x;
    pBuffer->end = (pBuffer->end + 1) % pBuffer->capacity;
    if (pBuffer->size == pBuffer->capacity - 1) {
        pBuffer->start = (pBuffer->start + 1) % pBuffer->capacity;
    }
    else {
        pBuffer->size++;
    }
    pBuffer->totalNumSamplesTaken++;
    _AdcBuffer_updateCurrentMean(pBuffer);
}

double AdcBuffer_getCurrentMean(AdcBuffer* pBuffer)
{
    double currentMean = -1.0;

    int lock_res = pthread_mutex_lock(&pBuffer->mutex);
    {
        if (lock_res != 0) {
            SYS_DIE("pthread_mutex_lock failed with rc = %d.\n", lock_res);
        }
        currentMean = pBuffer->currentMean;
    }
    int unlock_res = pthread_mutex_unlock(&pBuffer->mutex);
    if (unlock_res != 0) {
        SYS_DIE("pthread_mutex_unlock failed with rc = %d.\n", unlock_res);
    }

    return currentMean;
}

static void _AdcBuffer_updateCurrentMean(AdcBuffer* pBuffer)
{
    _AdcBuffer_verify(pBuffer);
    if (pBuffer->size == 0) return;
    double newMean;
    if (pBuffer->totalNumSamplesTaken == 1) {
        newMean = _AdcBuffer_get(pBuffer, 0);
    }
    else {
        newMean = (1.0 - ADC_BUFFER_PREV_SAMPLE_WEIGHT) * _AdcBuffer_get(pBuffer, pBuffer->size - 1) +
                    ADC_BUFFER_PREV_SAMPLE_WEIGHT * pBuffer->currentMean;
    }
    pBuffer->currentMean = newMean;
}

// Prints the contents of the buffer (not including "garbage" elements).
void AdcBuffer_print(AdcBuffer* pBuffer)
{
    LOG(LOG_LEVEL_DEBUG, "Contents of buffer:\n");
    for (uint64 i = 0; i < pBuffer->size; i++) {
        printf("%u, ", _AdcBuffer_get(pBuffer, i));
    }
    printf("\n\n");
}
// Prints the entire pBuffer->array.
void AdcBuffer_printArray(AdcBuffer* pBuffer)
{
    LOG(LOG_LEVEL_DEBUG, "Full contents of array:\n");
    for (uint64 i = 0; i < pBuffer->capacity; i++) {
        printf("%u, ", pBuffer->array[i]);
    }
    printf("\n\n");
}

void AdcBuffer_printProperties(AdcBuffer* pBuffer)
{
    LOG(LOG_LEVEL_DEBUG, "pBuffer = %p\n", pBuffer);
    LOG(LOG_LEVEL_DEBUG, "pBuffer->array = %p\n", pBuffer->array);
    LOG(LOG_LEVEL_DEBUG, "pBuffer->start = %llu\n", pBuffer->start);
    LOG(LOG_LEVEL_DEBUG, "pBuffer->end = %llu\n", pBuffer->end);
    LOG(LOG_LEVEL_DEBUG, "pBuffer->capacity = %llu\n", pBuffer->capacity);
    LOG(LOG_LEVEL_DEBUG, "pBuffer->size = %llu\n", pBuffer->size);
    printf("\n");
}

void AdcBuffer_printAll(AdcBuffer* pBuffer)
{
    AdcBuffer_printProperties(pBuffer);
    AdcBuffer_print(pBuffer);
    AdcBuffer_printArray(pBuffer);
}

// This function is not thread-safe.
void AdcBuffer_cleanup(AdcBuffer* pBuffer)
{
    assert(pBuffer);
    assert(pBuffer->array);
    free(pBuffer->array);
    pthread_mutex_destroy(&pBuffer->mutex);
}

// Verify that all important invariants for the data structure hold.
// Wouldn't need this function if C had classes (or if I made this a singleton-style module).
static void _AdcBuffer_verify(AdcBuffer* pBuffer)
{
    assert(pBuffer);
    assert(pBuffer->array);
    assert(pBuffer->capacity > 1);
    assert(pBuffer->size < pBuffer->capacity);
    assert(pBuffer->size <= pBuffer->totalNumSamplesTaken);
    assert(pBuffer->end < pBuffer->capacity);
    assert(pBuffer->start < pBuffer->capacity);
    if (pBuffer->size > 0) {
        assert(pBuffer->end != pBuffer->start);
    }
}