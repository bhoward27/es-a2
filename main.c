#include <stdio.h>

#include "adc.h"
#include "utils.h"
#include "gpio.h"
#include "i2c.h"
#include "return_val.h"
#include "log.h"
#include "digit_display.h"

int main(int argc, char* args[]) {
    initLogLevel();
    printf("Hello embedded world!\n");

    // Read from analog input once per second.
    // AIN1 Should be where my photoresistor stuff is connected to.
    // AIN0 is the zen cape's potentiometer
    while (1) {
        adc_in input0 = ADC_MAX_IN * 2;
        int res = Adc_readInput(0, &input0);
        if (res != OK) {
            LOG(LOG_LEVEL_WARN, "Uh oh.\n");
        }
        volt voltage = Adc_convertToVolts(input0);
        printf("AIN0\n");
        printf("ADC input = %u\n", input0);
        printf("In volts = %f\n", voltage);
        printf("\n");

        // Sleep for 1 second.
        sleepForMs(NUM_MS_PER_S);

        adc_in input1 = ADC_MAX_IN * 2;
        int res1 = Adc_readInput(1, &input1);
        if (res1 != OK) {
            LOG(LOG_LEVEL_WARN, "Uh oh.\n");
        }
        volt voltage1 = Adc_convertToVolts(input1);
        printf("AIN1\n");
        printf("ADC input = %u\n", input1);
        printf("In volts = %f\n", voltage1);
        printf("\n");

        // Sleep for 1 second.
        sleepForMs(NUM_MS_PER_S);
    }

    DigitDisplay_init();

    return 0;
}