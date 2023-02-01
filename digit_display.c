#include <stdbool.h>
#include <assert.h>

#include "digit_display.h"
#include "i2c.h"
#include "utils.h"
#include "log.h"

const DigitDisplayPattern DigitDisplay_digitPatterns[10] = {
    // 0
    {   DIGIT_DISPLAY_PATTERN_TOP_LEFT_VERTICAL | DIGIT_DISPLAY_PATTERN_TOP_HORIZONTAL |
            DIGIT_DISPLAY_PATTERN_TOP_RIGHT_VERTICAL,

        DIGIT_DISPLAY_PATTERN_BOTTOM_LEFT_VERTICAL | DIGIT_DISPLAY_PATTERN_BOTTOM_HORIZONTAL |
            DIGIT_DISPLAY_PATTERN_BOTTOM_RIGHT_VERTICAL
    },

    // 1
    {   DIGIT_DISPLAY_PATTERN_TOP_RIGHT_DIAGONAL | DIGIT_DISPLAY_PATTERN_TOP_RIGHT_VERTICAL,

        DIGIT_DISPLAY_PATTERN_BOTTOM_RIGHT_VERTICAL
    },

    // 2
    {   DIGIT_DISPLAY_PATTERN_TOP_HORIZONTAL | DIGIT_DISPLAY_PATTERN_TOP_RIGHT_VERTICAL |
            DIGIT_DISPLAY_PATTERN_TOP_MID_LEFT_HORIZONTAL,

        DIGIT_DISPLAY_PATTERN_BOTTOM_MID_RIGHT_HORIZONTAL | DIGIT_DISPLAY_PATTERN_BOTTOM_LEFT_VERTICAL |
            DIGIT_DISPLAY_PATTERN_BOTTOM_HORIZONTAL
    },

    // 3
    {   DIGIT_DISPLAY_PATTERN_TOP_HORIZONTAL | DIGIT_DISPLAY_PATTERN_TOP_RIGHT_VERTICAL,

        DIGIT_DISPLAY_PATTERN_BOTTOM_MID_RIGHT_HORIZONTAL | DIGIT_DISPLAY_PATTERN_BOTTOM_RIGHT_VERTICAL |
            DIGIT_DISPLAY_PATTERN_BOTTOM_HORIZONTAL
    },

    // 4
    {
        DIGIT_DISPLAY_PATTERN_TOP_LEFT_VERTICAL | DIGIT_DISPLAY_PATTERN_TOP_RIGHT_VERTICAL |
            DIGIT_DISPLAY_PATTERN_TOP_MID_LEFT_HORIZONTAL,

        DIGIT_DISPLAY_PATTERN_BOTTOM_MID_RIGHT_HORIZONTAL | DIGIT_DISPLAY_PATTERN_BOTTOM_RIGHT_VERTICAL
    },

    // 5


    // 6


    // 7


    // 8


    // 9

};

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

    I2c_enable(I2c_bus1GpioPinInfo, DIGIT_DISPLAY_I2C_BUS_NUMBER, I2C_BUS_1_GPIO_EXTENDER_ADDRESS);

    // TODO: Remove.
    DigitDisplay_enable(DIGIT_DISPLAY_LEFT);
    for (int shifts = 0; shifts < 8; shifts++) {
        uint8 bitPattern = (1u << shifts);
        LOG(LOG_LEVEL_DEBUG, "Shifts = %d, Bit pattern (decimal) = %u\n\n", shifts, bitPattern);
        I2c_write(DIGIT_DISPLAY_I2C_BUS_NUMBER,
                I2C_BUS_1_GPIO_EXTENDER_ADDRESS,
                DIGIT_DISPLAY_TOP_I2C_REGISTER_ADDRESS,
                bitPattern);
        sleepForMs(NUM_MS_PER_S * 10);
    }
    I2c_write(DIGIT_DISPLAY_I2C_BUS_NUMBER,
                I2C_BUS_1_GPIO_EXTENDER_ADDRESS,
                DIGIT_DISPLAY_TOP_I2C_REGISTER_ADDRESS,
                0);

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