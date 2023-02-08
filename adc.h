// Implements core functions related to reading input from the ADC (Analog to Digital Converter).
#ifndef ADC_H_
#define ADC_H_

#include "int_typedefs.h"

#define ADC_FILE_PATH_FORMAT "/sys/bus/iio/devices/iio:device0/in_voltage%u_raw"

#define ADC_MIN_IN 0
#define ADC_MAX_IN 4095

#define ADC_GROUND_VOLTAGE 0
#define ADC_MAX_VOLTAGE 1.8f

#define ADC_MIN_ANALOG_INPUT_NUM 0
#define ADC_MAX_ANALOG_INPUT_NUM 6

typedef uint16 adc_in;
typedef double volt;

volt Adc_convertToVolts(double x);
int Adc_readInput(uint8 analogInputNum, adc_in* pAdcInput);

#endif