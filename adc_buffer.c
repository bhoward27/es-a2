#include <assert.h>
#include <stddef.h>
#include <stdlib.h>

#include "adc_buffer.h"
#include "log.h"

static void AdcBuffer_verify(AdcBuffer* pBuffer);

void AdcBuffer_init(AdcBuffer* pBuffer, uint64 n)
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
}

// The 'size' in resize is moreso referring to AdcBuffer.capacity
void AdcBuffer_resize(AdcBuffer* pBuffer, uint64 m)
{
    AdcBuffer_verify(pBuffer);
    assert(m > 0);

    uint64 newCapacity = m + 1;

    if (newCapacity == pBuffer->capacity) {
        return;
    }

    AdcBuffer newBuffer;
    AdcBuffer_init(&newBuffer, m);

    // Copy as many of the newest samples as we can fit into newBuffer.
    // NOTE: Could use memcpy instead for some more efficient yet uglier code.
    uint64 newMaxSize = newCapacity - 1;
    uint64 numSamplesToShed = (newMaxSize < pBuffer->size) ? pBuffer->size - newMaxSize : 0;
    for (uint64 i = numSamplesToShed; i < pBuffer->size; i++) {
        AdcBuffer_add(&newBuffer, AdcBuffer_get(pBuffer, i));
    }

    free(pBuffer->array);
    *pBuffer = newBuffer;
}

adc_in AdcBuffer_get(AdcBuffer* pBuffer, uint64 i)
{
    AdcBuffer_verify(pBuffer);

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
    assert(x >= ADC_MIN_IN && x <= ADC_MAX_IN);
    AdcBuffer_verify(pBuffer);

    // Overwrite whatever was here.
    pBuffer->array[pBuffer->end] = x;
    pBuffer->end = (pBuffer->end + 1) % pBuffer->capacity;
    if (pBuffer->size == pBuffer->capacity - 1) {
        pBuffer->start = (pBuffer->start + 1) % pBuffer->capacity;
    }
    else {
        pBuffer->size++;
    }
}

// Prints the contents of the buffer (not including "garbage" elements).
void AdcBuffer_print(AdcBuffer* pBuffer)
{
    LOG(LOG_LEVEL_DEBUG, "Contents of buffer:\n");
    for (uint64 i = 0; i < pBuffer->size; i++) {
        printf("%u, ", AdcBuffer_get(pBuffer, i));
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

void AdcBuffer_cleanup(AdcBuffer* pBuffer)
{
    assert(pBuffer);
    assert(pBuffer->array);
    free(pBuffer->array);
}

// Verify that all important invariants for the data structure hold.
// Wouldn't need this function if C had classes (or if I made this a singleton-style module).
static void AdcBuffer_verify(AdcBuffer* pBuffer)
{
    assert(pBuffer);
    assert(pBuffer->array);
    assert(pBuffer->capacity > 1);
    assert(pBuffer->size < pBuffer->capacity);
    assert(pBuffer->end < pBuffer->capacity);
    assert(pBuffer->start < pBuffer->capacity);
    if (pBuffer->size > 0) {
        assert(pBuffer->end != pBuffer->start);
    }
}