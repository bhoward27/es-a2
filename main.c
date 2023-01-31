#include <stdio.h>

#include "adc.h"
#include "utils.h"
#include "gpio.h"
#include "i2c.h"
#include "return_val.h"
#include "log.h"

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

    int res1 = Gpio_precheckSetPinMode(I2C_BUS_1_GPIO_HEADER, I2C_BUS_1_GPIO_DATA_PIN, "i2c", GPIO_MAX_MODE_LEN);
    int res2 = Gpio_precheckSetPinMode(I2C_BUS_1_GPIO_HEADER, I2C_BUS_1_GPIO_CLOCK_PIN, "i2c", GPIO_MAX_MODE_LEN);
    if (res1 == COMMAND_SUCCESS && res2 == COMMAND_SUCCESS) {
        LOG(LOG_LEVEL_DEBUG, "Successfully set pins to i2c mode.\n");
    }
    else {
        LOG(LOG_LEVEL_ERROR, "Failed to set pins to i2c mode.\n");
    }

    return 0;
}