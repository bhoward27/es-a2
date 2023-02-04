#ifndef LIGHT_METER_H_
#define LIGHT_METER_H_

#include "adc.h"

#define LIGHT_METER_ANALOG_INPUT_NUM 1

adc_in LightMeter_read(void);

#endif