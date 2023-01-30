#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "adc.h"
#include "utils.h"
#include "log.h"

volt Adc_convertToVolts(adc_in x)
{
    assert(x >= ADC_MIN_IN && x <= ADC_MAX_IN);

    return ((volt) x / ADC_MAX_IN) * ADC_MAX_VOLTAGE;
}

adc_in Adc_readInput(uint8 analogInputNum)
{
    assert(analogInputNum >= ADC_MIN_ANALOG_INPUT_NUM && analogInputNum <= ADC_MAX_ANALOG_INPUT_NUM);

    char filePath[MEDIUM_STRING_LEN];
    snprintf(filePath, MEDIUM_STRING_LEN, ADC_FILE_PATH_FORMAT, analogInputNum);
    const int maxNumDigits = 4;
    char buffer[maxNumDigits + 1];
    // TODO: The closing and opening of the file might not be too efficient if I'm trying to read from the ADC a lot.
    // So could change this so that file gets opened in Adc_init() and closed in Adc_close() -- just read in this func.
    readFile(filePath, buffer, maxNumDigits, 1, true);
    buffer[maxNumDigits] = '\0';
    adc_in adcInput = atoi(buffer);

    if (adcInput < ADC_MIN_IN || adcInput > ADC_MAX_IN) {
        LOG(LOG_LEVEL_ERROR, "Read invalid ADC input '%u' from '%s'.\n", adcInput, filePath);
    }

    return adcInput;
}