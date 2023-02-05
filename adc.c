#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "adc.h"
#include "utils.h"
#include "log.h"

volt Adc_convertToVolts(double x)
{
    assert(x >= ADC_MIN_IN && x <= ADC_MAX_IN);

    return ((volt) x / ADC_MAX_IN) * ADC_MAX_VOLTAGE;
}

int Adc_readInput(uint8 analogInputNum, adc_in* pAdcInput)
{
    assert(analogInputNum >= ADC_MIN_ANALOG_INPUT_NUM && analogInputNum <= ADC_MAX_ANALOG_INPUT_NUM);

    char filePath[MEDIUM_STRING_LEN];
    snprintf(filePath, MEDIUM_STRING_LEN, ADC_FILE_PATH_FORMAT, analogInputNum);
    FILE* pFile = fopen(filePath, "r");
    if (pFile == NULL) {
        FILE_OPEN_ERR(filePath, false);
        return ERR_OPEN;
    }

    int res = fscanf(pFile, "%hu", pAdcInput);
    if (res != 1) {
        FILE_READ_ERR(filePath, false);
        if (fclose(pFile)) {
            FILE_CLOSE_ERR(filePath, false);
        }
        return ERR_READ;
    }

    if (fclose(pFile)) {
        FILE_CLOSE_ERR(filePath, false);
        return ERR_CLOSE;
    }

    if (*pAdcInput < ADC_MIN_IN || *pAdcInput > ADC_MAX_IN) {
        LOG(LOG_LEVEL_WARN, "Read invalid ADC input '%u' from '%s'.\n", *pAdcInput, filePath);
        return ERR_INVALID_ADC_INPUT;
    }

    return OK;
}