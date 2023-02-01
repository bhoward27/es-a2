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

    // Read from AIN1 once per second.
    // while (1) {
    //     uint8 analogInputNum = 1;
    //     adc_in input = Adc_readInput(analogInputNum);
    //     volt voltage = Adc_convertToVolts(input);
    //     printf("ADC input = %u\n", input);
    //     printf("In volts = %f\n", voltage);

    //     // Sleep for 1 second.
    //     sleepForMs(NUM_MS_PER_S);
    // }

    DigitDisplay_init();

    return 0;
}