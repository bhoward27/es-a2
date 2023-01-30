#ifndef UTILS_H_
#define UTILS_H_

#include <stdbool.h>

#include "int_typedefs.h"
#include "return_val.h"

#define GPIO_READ_ERR -1

// TODO: Move all GPIO stuff to its own module.
#define GPIO_EXPORT_PATH "/sys/class/gpio/export"
#define GPIO_PIN_PATH_PREFIX "/sys/class/gpio/gpio"
#define GPIO_CONFIG_PIN_PATH "/usr/bin/config-pin"

#define DEFAULT_STRING_LEN 1024

#define NUM_MS_PER_S 1000

typedef uint8 GpioNum;

int overwriteFile(char* filePath, char* string, bool exitOnFailure);
int readFile(char* filePath, void* outData, size_t numBytesPerItem, size_t numItems, bool exitOnFailure);
int runCommand(char* command);
int64 getTimeInMs(void);
void sleepForMs(int64 delayInMs);
void Gpio_exportPin(GpioNum pin, char* header, GpioNum linuxPin);
void Gpio_configIo(GpioNum linuxPin, bool isInput);
void Gpio_initPin(GpioNum pin, char* header, GpioNum linuxPin, bool isInput);
int Gpio_readInput(GpioNum linuxPin);

#endif