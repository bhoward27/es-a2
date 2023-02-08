// Provides the LightMeter_read() function, which reads the photo resistor's voltage via the ADC. The function then
// returns an integer value from 0 to 4095 produced by the ADC.
#ifndef LIGHT_METER_H_
#define LIGHT_METER_H_

#include "adc.h"

#define LIGHT_METER_ANALOG_INPUT_NUM 1

adc_in LightMeter_read(void);

#endif