#include <stdio.h>

#include "adc.h"
#include "utils.h"
#include "gpio.h"
#include "i2c.h"
#include "return_val.h"

int main(int argc, char* args[]) {
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

    char mode[SMALL_STRING_LEN];
    int res = Gpio_queryPinMode(I2C_BUS_1_GPIO_HEADER, I2C_BUS_1_GPIO_DATA_PIN, mode, SMALL_STRING_LEN);
    if (res == COMMAND_SUCCESS) {
        printf("GPIO %s.%s mode = '%s'\n", I2C_BUS_1_GPIO_HEADER, I2C_BUS_1_GPIO_DATA_PIN, mode);
    }

    // TODO: Test if setPinMode works and test setting it only if current pin mode isn't what I want.

    return 0;
}