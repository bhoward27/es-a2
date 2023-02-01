#include <stdbool.h>

#include "i2c.h"
#include "utils.h"
#include "log.h"
#include "int_typedefs.h"

const GpioInfo I2c_bus1GpioPinInfo[I2C_BUS_NUM_PINS] = {
    {I2C_BUS_1_GPIO_HEADER, I2C_BUS_1_GPIO_DATA_PIN},
    {I2C_BUS_1_GPIO_HEADER, I2C_BUS_1_GPIO_CLOCK_PIN}
};

// TODO: Change I2C to write directly to files instead of using i2cset command. Would likely be more efficient.
// See Brian's I2C guide for how to do that.
void I2c_enable(const GpioInfo busGpioInfo[], uint8 busNumber, uint8 gpioExtenderAddress)
{
    int numSuccessful = 0;
    for (int i = 0; i < I2C_BUS_NUM_PINS; i++) {
        int res = Gpio_precheckSetPinMode(busGpioInfo[i].header, busGpioInfo[i].pin, "i2c", GPIO_MAX_MODE_LEN);
        if (res == COMMAND_SUCCESS) numSuccessful++;
    }
    // if (numSuccessful == I2C_BUS_NUM_PINS) {
    //     LOG(LOG_LEVEL_DEBUG, "%s(%p) SUCCEEDED.\n", __func__, (void*) busGpioInfo);
    // }
    // else {
    //     LOG(LOG_LEVEL_WARN, "%s(%p) FAILED.\n", __func__, (void*) busGpioInfo);
    // }
    if (numSuccessful != I2C_BUS_NUM_PINS) return;

    // Enable output on all pins for GPIO extender.
    numSuccessful = 0;
    for (uint8 i = 0; i < 2; i++) {
        // Write zeroes to register address 0x00 and 0x01. (NOTE: Zed Cape Red would need different addresses).
        int res = I2c_write(busNumber, gpioExtenderAddress, i, 0x00);
        if (res == COMMAND_SUCCESS) numSuccessful++;
    }
    if (numSuccessful == I2C_BUS_NUM_PINS) {
        LOG(LOG_LEVEL_DEBUG, "%s(%p, %u, %u) SUCCEEDED.\n", __func__, busGpioInfo, busNumber, gpioExtenderAddress);
    }
    else {
        LOG(LOG_LEVEL_WARN, "%s(%p, %u, %u) FAILED.\n", __func__, busGpioInfo, busNumber, gpioExtenderAddress);
    }
}


int I2c_write(uint8 busNumber, uint8 deviceAddress, uint8 registerAddress, uint8 value)
{
    char command[MEDIUM_STRING_LEN];
    int snprintf(char *str, size_t size, const char *format, ...);
    snprintf(command,
             MEDIUM_STRING_LEN,
             "%s -y %u 0x%x 0x%x 0x%x",
             I2C_WRITE_COMMAND,
             busNumber,
             deviceAddress,
             registerAddress,
             value);
    return runCommand(command);
}
