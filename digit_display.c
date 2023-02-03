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
    {   DIGIT_DISPLAY_PATTERN_TOP_LEFT_VERTICAL | DIGIT_DISPLAY_PATTERN_TOP_RIGHT_VERTICAL |
            DIGIT_DISPLAY_PATTERN_TOP_MID_LEFT_HORIZONTAL,

        DIGIT_DISPLAY_PATTERN_BOTTOM_MID_RIGHT_HORIZONTAL | DIGIT_DISPLAY_PATTERN_BOTTOM_RIGHT_VERTICAL
    },

    // 5
    {   DIGIT_DISPLAY_PATTERN_TOP_HORIZONTAL | DIGIT_DISPLAY_PATTERN_TOP_LEFT_VERTICAL |
            DIGIT_DISPLAY_PATTERN_TOP_MID_LEFT_HORIZONTAL,

        DIGIT_DISPLAY_PATTERN_BOTTOM_MID_RIGHT_HORIZONTAL | DIGIT_DISPLAY_PATTERN_BOTTOM_RIGHT_VERTICAL |
            DIGIT_DISPLAY_PATTERN_BOTTOM_HORIZONTAL
    },

    // 6
    {   DIGIT_DISPLAY_PATTERN_TOP_HORIZONTAL | DIGIT_DISPLAY_PATTERN_TOP_LEFT_VERTICAL |
            DIGIT_DISPLAY_PATTERN_TOP_MID_LEFT_HORIZONTAL,

        DIGIT_DISPLAY_PATTERN_BOTTOM_MID_RIGHT_HORIZONTAL | DIGIT_DISPLAY_PATTERN_BOTTOM_LEFT_VERTICAL |
            DIGIT_DISPLAY_PATTERN_BOTTOM_HORIZONTAL | DIGIT_DISPLAY_PATTERN_BOTTOM_RIGHT_VERTICAL
    },

    // 7
    {   DIGIT_DISPLAY_PATTERN_TOP_HORIZONTAL | DIGIT_DISPLAY_PATTERN_TOP_RIGHT_DIAGONAL,

        DIGIT_DISPLAY_PATTERN_BOTTOM_MID_VERTICAL
    },

    // 8
    {   DIGIT_DISPLAY_PATTERN_TOP_LEFT_VERTICAL | DIGIT_DISPLAY_PATTERN_TOP_HORIZONTAL |
            DIGIT_DISPLAY_PATTERN_TOP_MID_LEFT_HORIZONTAL | DIGIT_DISPLAY_PATTERN_TOP_RIGHT_VERTICAL,

        DIGIT_DISPLAY_PATTERN_BOTTOM_MID_RIGHT_HORIZONTAL | DIGIT_DISPLAY_PATTERN_BOTTOM_LEFT_VERTICAL |
            DIGIT_DISPLAY_PATTERN_BOTTOM_HORIZONTAL | DIGIT_DISPLAY_PATTERN_BOTTOM_RIGHT_VERTICAL
    },

    // 9
    {   DIGIT_DISPLAY_PATTERN_TOP_LEFT_VERTICAL | DIGIT_DISPLAY_PATTERN_TOP_HORIZONTAL |
            DIGIT_DISPLAY_PATTERN_TOP_RIGHT_VERTICAL | DIGIT_DISPLAY_PATTERN_TOP_MID_LEFT_HORIZONTAL,

        DIGIT_DISPLAY_PATTERN_BOTTOM_MID_RIGHT_HORIZONTAL | DIGIT_DISPLAY_PATTERN_BOTTOM_RIGHT_VERTICAL
    }
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
    // DigitDisplay_enable(DIGIT_DISPLAY_LEFT);
    // DigitDisplay_enable(DIGIT_DISPLAY_RIGHT);
    // for (uint8 digit = 0; digit <= 9; digit++) {
    //     LOG(LOG_LEVEL_DEBUG, "%u\n", digit);
    //     DigitDisplay_digit(digit);
    //     sleepForMs(NUM_MS_PER_S);
    // }
    // DigitDisplay_disable(DIGIT_DISPLAY_RIGHT);
    // sleepForMs(NUM_MS_PER_S * 5);

    const int64 holdTimeMs = 1000;
    for (uint8 i = 0; i <= 105; i++) {
        LOG(LOG_LEVEL_DEBUG, "%u\n", i);
        int64 currentDuration = 0;
        int64 startTime = getTimeInMs();
        uint64 count = 0;
        while (currentDuration <= holdTimeMs) {
            DigitDisplay_number(i);
            currentDuration = getTimeInMs() - startTime;
            count++;
            LOG(LOG_LEVEL_INFO, "`\n");
        }
        LOG(LOG_LEVEL_INFO, "***********************************\n");

    }
    DigitDisplay_disable(DIGIT_DISPLAY_LEFT);
    DigitDisplay_disable(DIGIT_DISPLAY_RIGHT);

    // for (int shifts = 0; shifts < 8; shifts++) {
    //     uint8 bitPattern = (1u << shifts);
    //     LOG(LOG_LEVEL_DEBUG, "Shifts = %d, Bit pattern (decimal) = %u\n\n", shifts, bitPattern);
    //     I2c_write(DIGIT_DISPLAY_I2C_BUS_NUMBER,
    //             I2C_BUS_1_GPIO_EXTENDER_ADDRESS,
    //             DIGIT_DISPLAY_TOP_I2C_REGISTER_ADDRESS,
    //             bitPattern);
    //     sleepForMs(NUM_MS_PER_S * 10);
    // }
    // I2c_write(DIGIT_DISPLAY_I2C_BUS_NUMBER,
    //             I2C_BUS_1_GPIO_EXTENDER_ADDRESS,
    //             DIGIT_DISPLAY_TOP_I2C_REGISTER_ADDRESS,
    //             0);

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

// This function assumes the appropriate display half and I2C bus have already been fully enabled.
void DigitDisplay_digit(uint8 digit)
{
    assert(digit >= 0 && digit <= 9);

    I2c_write(DIGIT_DISPLAY_I2C_BUS_NUMBER,
              I2C_BUS_1_GPIO_EXTENDER_ADDRESS,
              DIGIT_DISPLAY_TOP_I2C_REGISTER_ADDRESS,
              DigitDisplay_digitPatterns[digit].topBitPattern);

    I2c_write(DIGIT_DISPLAY_I2C_BUS_NUMBER,
              I2C_BUS_1_GPIO_EXTENDER_ADDRESS,
              DIGIT_DISPLAY_BOTTOM_I2C_REGISTER_ADDRESS,
              DigitDisplay_digitPatterns[digit].bottomBitPattern);
}

// This function assumes the appropriate display half and I2C bus have already been fully enabled.
void DigitDisplay_allSegmentsOff(void)
{
    I2c_write(DIGIT_DISPLAY_I2C_BUS_NUMBER,
              I2C_BUS_1_GPIO_EXTENDER_ADDRESS,
              DIGIT_DISPLAY_TOP_I2C_REGISTER_ADDRESS,
              DIGIT_DISPLAY_PATTERN_ALL_OFF);

    I2c_write(DIGIT_DISPLAY_I2C_BUS_NUMBER,
              I2C_BUS_1_GPIO_EXTENDER_ADDRESS,
              DIGIT_DISPLAY_BOTTOM_I2C_REGISTER_ADDRESS,
              DIGIT_DISPLAY_PATTERN_ALL_OFF);
}

// Display a number from 0 to 99.
void DigitDisplay_number(uint8 number)
{
    if (number > DIGIT_DISPLAY_MAX_NUM) {
        number = DIGIT_DISPLAY_MAX_NUM;
    }

    uint8 ones = number % 10;
    uint8 tens = number / 10;
    const int64 sleepTime = 5;

    // Display tens on left side.
    DigitDisplay_disable(DIGIT_DISPLAY_RIGHT);
    DigitDisplay_digit(tens);
    DigitDisplay_enable(DIGIT_DISPLAY_LEFT);
    sleepForMs(sleepTime);
    // busyWaitForMs(sleepTime);

    // Display ones on right side.
    DigitDisplay_disable(DIGIT_DISPLAY_LEFT);
    DigitDisplay_digit(ones);
    DigitDisplay_enable(DIGIT_DISPLAY_RIGHT);
    sleepForMs(sleepTime);
    // busyWaitForMs(sleepTime);

    // When sleeping 5 ms each, in 1 second loop would expect
    // this function to be executed around 100 times.
    // INTSTEAD, on -Og get only about 17 invocations. BAD.
    // When sleeping 1 ms each, get only 20 invocations in 1 second loop.
    // Using the busyWaitForMs function made absolutely no difference, so it's not the sleep
    // function causing this at all.
    // TODO: Make I2C write function more efficient. Don't use I2C-set. See if that
    // fixes it.
}