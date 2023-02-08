// Implements the AdcStats_dips() functions, which calclulates the number of dips below avg value in samples that
// occurred. Uses hysteresis to manage the noise which will likely come from the ADC.
#ifndef ADC_STATS_H
#define ADC_STATS_H

#include "adc.h"

#define DIP_ENTER_THRESHOLD_VOLTS 0.1
#define DIP_HYSTERESIS_VOLTS 0.03
#define DIP_EXIT_THRESHOLD_VOLTS (DIP_ENTER_THRESHOLD_VOLTS - DIP_HYSTERESIS_VOLTS)

uint64 AdcStats_dips(adc_in* samples, uint64 numSamples, double mean);

#endif