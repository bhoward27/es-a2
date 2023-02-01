#ifndef DIGIT_DISPLAY_H_
#define DIGIT_DISPLAY_H_

#include "gpio.h"

#define DIGIT_DISPLAY_GPIO_HEADER "p8"
#define DIGIT_DISPLAY_LEFT_GPIO_PIN "26"
#define DIGIT_DISPLAY_RIGHT_GPIO_PIN "27"

#define DIGIT_DISPLAY_LEFT_GPIO_LINUX_PIN "61"
#define DIGIT_DISPLAY_RIGHT_GPIO_LINUX_PIN "44"

extern const GpioLinuxInfo DigitDisplay_gpioLeft;
extern const GpioLinuxInfo DigitDisplay_gpioRight;

typedef enum {
    DIGIT_DISPLAY_LEFT,
    DIGIT_DISPLAY_RIGHT
} DigitDisplayHalf;

void DigitDisplay_init(void);
void DigitDisplay_enable(DigitDisplayHalf displayHalf);
void DigitDisplay_disable(DigitDisplayHalf displayHalf);

#endif