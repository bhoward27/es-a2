#include <assert.h>
#include <stdbool.h>

#include "adc_stats.h"

// mean should be a floating-point value between [ADC_IN_MIN, ADC_IN_MAX]
uint64 AdcStats_dips(adc_in* samples, uint64 numSamples, double mean)
{
    assert(samples);
    if (numSamples == 0) return 0;

    volt meanV = Adc_convertToVolts(mean);

    bool isInDip = false;
    bool isPrevSampleInDip = false;
    uint64 dips = 0;
    for (uint64 i = 0; i < numSamples; i++) {
        volt sampleV = Adc_convertToVolts(samples[i]);
        if (sampleV < meanV - DIP_ENTER_THRESHOLD_VOLTS) {
            isInDip = true;
        }
        else if (sampleV > meanV + DIP_EXIT_THRESHOLD_VOLTS) {
            isInDip = false;
        }

        if (!isPrevSampleInDip && isInDip) {
            dips++;
        }

        isPrevSampleInDip = isInDip;
    }

    return dips;
}