#include <stdbool.h>

#include "light_meter.h"

static bool initialized = false;

void LightMeter_init(void)
{
    if (!initialized) {
        // TODO: Do init procedure.

        initialized = true;
    }
}


void LightMeter_cleanup(void)
{
    // TODO: Do cleanup.


    initialized = false;
}

adc_in LightMeter_read(void)
{
    // Set to an invalid value to make it easier to check for errors.
    adc_in brightness = ADC_MAX_IN * 2;
    Adc_readInput(LIGHT_METER_ANALOG_INPUT_NUM, &brightness);
    return brightness;
}
