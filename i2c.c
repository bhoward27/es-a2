#include <stdbool.h>

#include "i2c.h"
#include "utils.h"
#include "log.h"

const GpioInfo I2c_bus1GpioInfo[I2C_BUS_NUM_PINS] = {
    {I2C_BUS_1_GPIO_HEADER, I2C_BUS_1_GPIO_DATA_PIN},
    {I2C_BUS_1_GPIO_HEADER, I2C_BUS_1_GPIO_CLOCK_PIN}
};

void I2c_enable(const GpioInfo busGpioInfo[])
{
    int numSuccessful = 0;
    for (int i = 0; i < I2C_BUS_NUM_PINS; i++) {
        int res = Gpio_precheckSetPinMode(busGpioInfo[i].header, busGpioInfo[i].pin, "i2c", GPIO_MAX_MODE_LEN);
        if (res == COMMAND_SUCCESS) numSuccessful++;
    }
    if (numSuccessful == I2C_BUS_NUM_PINS) {
        LOG(LOG_LEVEL_DEBUG, "%s(%p) SUCCEEDED.\n", __func__, (void*) busGpioInfo);
    }
    else {
        LOG(LOG_LEVEL_WARN, "%s(%p) FAILED.\n", __func__, (void*) busGpioInfo);
    }
}


// TODO: i2c_write