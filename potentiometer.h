// Provides the Potentiometer_read() function, which does exactly what you think it does.
#ifndef POTENTIOMETER_H_
#define POTENTIOMETER_H_

#include "adc.h"

#define POTENTIOMETER_ANALOG_INPUT_NUM 0

adc_in Potentiometer_read(void);

#endif