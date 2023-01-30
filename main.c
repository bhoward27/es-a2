#include <stdio.h>

#include "adc.h"
#include "utils.h"

int main(int argc, char* args[]) {
    printf("Hello embedded world!\n");

    while (1) {
        uint8 analogInputNum = 1;
        adc_in input = Adc_readInput(analogInputNum);
        volt voltage = Adc_convertToVolts(input);
        printf("ADC input = %u\n", input);
        printf("In volts = %f\n", voltage);

        // Sleep for 1 second.
        sleepForMs(NUM_MS_PER_S);
    }

    return 0;
}