#include "potentiometer.h"

adc_in Potentiometer_read(void)
{
    // Set to an invalid value to make it easier to check for errors.
    adc_in input = ADC_MAX_IN * 2;
    Adc_readInput(POTENTIOMETER_ANALOG_INPUT_NUM, &input);
    return input;
}