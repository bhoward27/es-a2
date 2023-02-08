// This modules samples the brightness from the photo resistor once each ms (or at least, that's the goal).
#ifndef LIGHT_SAMPLER_H_
#define LIGHT_SAMPLER_H_

#include "adc_buffer.h"

void LightSampler_init(AdcBuffer* pBuffer);
void LightSampler_waitForShutdown(void);

#endif