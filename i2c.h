#ifndef I2C_H_
#define I2C_H_

#include "gpio.h"

// We assume each I2C bus has only two pins: data and clock.
#define I2C_BUS_NUM_PINS 2

#define I2C_BUS_1_GPIO_HEADER "p9"
#define I2C_BUS_1_GPIO_DATA_PIN "18"
#define I2C_BUS_1_GPIO_CLOCK_PIN "17"

// TODO: Should this go into an array, struct, no? what do?
// probably struct with GpioInfo array as first element.
// Also make header a member instead of element, since it's repeated otherwise.
// So only array elements are the pins.
#define I2C_BUS_1_GPIO_EXTENDER_ADDRESS "0x20"

extern const GpioInfo I2c_bus1GpioInfo[I2C_BUS_NUM_PINS];

void I2c_enable(const GpioInfo busGpioInfo[]);

#endif