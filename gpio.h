#ifndef GPIO_H_
#define GPIO_H_

#include <stddef.h>

#include "int_typedefs.h"

#define GPIO_READ_ERR -1

#define GPIO_EXPORT_PATH "/sys/class/gpio/export"
#define GPIO_PIN_PATH_PREFIX "/sys/class/gpio/gpio"
#define GPIO_CONFIG_PIN_PATH "/usr/bin/config-pin"

int Gpio_setPinMode(const char* header, const char* pin, const char* mode);
int Gpio_queryPinMode(const char* header, const char* pin, char* outMode, size_t modeMaxLen);

#endif