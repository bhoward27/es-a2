// This module prints information regarding the ADC buffer passed in.
// It also resizes the buffer based on current potentiometer input.
#ifndef CONSOLE_H_
#define CONSOLE_H_

#include "adc_buffer.h"

void Console_init(AdcBuffer* pBuffer);
void Console_waitForShutdown(void);
void* Console_run(void* args);

#endif
