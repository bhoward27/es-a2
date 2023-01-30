#ifndef UTILS_H_
#define UTILS_H_

#include <stdbool.h>

#include "int_typedefs.h"
#include "return_val.h"

#define SMALL_STRING_LEN 128
#define MEDIUM_STRING_LEN 1024

#define NUM_MS_PER_S 1000

int overwriteFile(char* filePath, char* string, bool exitOnFailure);
int readFile(char* filePath, void* outData, size_t numBytesPerItem, size_t numItems, bool exitOnFailure);
int runCommand(const char* command);
int64 getTimeInMs(void);
void sleepForMs(int64 delayInMs);
// void Gpio_exportPin(GpioNum pin, char* header, GpioNum linuxPin);
// void Gpio_configIo(GpioNum linuxPin, bool isInput);
// void Gpio_initPin(GpioNum pin, char* header, GpioNum linuxPin, bool isInput);
// int Gpio_readInput(GpioNum linuxPin);

#endif