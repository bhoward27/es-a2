#ifndef LIGHT_SAMPLER_H_
#define LIGHT_SAMPLER_H_

#include "adc_buffer.h"

void LightSampler_init(AdcBuffer* pBuffer);
void* LightSampler_run(void* args);
void LightSampler_waitForShutdown(void);

#endif