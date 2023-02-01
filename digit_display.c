#include <stdbool.h>
#include <assert.h>

#include "digit_display.h"
#include "i2c.h"
#include "utils.h"

const GpioLinuxInfo DigitDisplay_gpioLeft = {
    {DIGIT_DISPLAY_GPIO_HEADER, DIGIT_DISPLAY_LEFT_GPIO_PIN},
    DIGIT_DISPLAY_LEFT_GPIO_LINUX_PIN
};
const GpioLinuxInfo DigitDisplay_gpioRight = {
    {DIGIT_DISPLAY_GPIO_HEADER, DIGIT_DISPLAY_RIGHT_GPIO_PIN},
    DIGIT_DISPLAY_RIGHT_GPIO_LINUX_PIN
};

static bool initialized = false;

static void DigitDisplay_drive(DigitDisplayHalf displayHalf, uint8 value);

void DigitDisplay_init(void)
{
    if (initialized) return;

    Gpio_exportPin(DigitDisplay_gpioLeft.linuxPin);
    Gpio_exportPin(DigitDisplay_gpioRight.linuxPin);

    // Set pins' direction to out.
    Gpio_configIo(DigitDisplay_gpioLeft.linuxPin, false);
    Gpio_configIo(DigitDisplay_gpioRight.linuxPin, false);

    I2c_enable(I2c_bus1GpioInfo);

    initialized = true;
}

static void DigitDisplay_drive(DigitDisplayHalf displayHalf, uint8 value)
{
    switch (displayHalf) {
        case DIGIT_DISPLAY_LEFT:
            Gpio_write(DigitDisplay_gpioLeft.linuxPin, value);
            break;
        case DIGIT_DISPLAY_RIGHT:
            Gpio_write(DigitDisplay_gpioRight.linuxPin, value);
            break;
        default:
            assert(false);
            break;
    }
}

void DigitDisplay_enable(DigitDisplayHalf displayHalf)
{
    DigitDisplay_drive(displayHalf, 1);
}

void DigitDisplay_disable(DigitDisplayHalf displayHalf)
{
    DigitDisplay_drive(displayHalf, 0);
}