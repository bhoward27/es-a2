#include <time.h>
#include <sys/wait.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <assert.h>

#include "utils.h"
#include "log.h"

int overwriteFile(const char* filePath, const char* string, bool exitOnFailure)
{
    assert(filePath != NULL);
    assert(string != NULL);

    FILE* pFile = fopen(filePath, "w");
    if (pFile == NULL) {
        FILE_OPEN_ERR(filePath, exitOnFailure);
        return ERR_OPEN;
    }
    int res = fprintf(pFile, string);
    if (res <= 0) {
        FILE_WRITE_ERR(filePath, exitOnFailure);
        return ERR_WRITE;
    }

    if (fclose(pFile)) {
        FILE_CLOSE_ERR(filePath, exitOnFailure);
        return ERR_CLOSE;
    }

    return OK;
}

int readFile(char* filePath, void* outData, size_t numBytesPerItem, size_t numItems, bool exitOnFailure)
{
    FILE* pFile = fopen(filePath, "r");
    if (pFile == NULL) {
        if (exitOnFailure)
        FILE_OPEN_ERR(filePath, exitOnFailure);
        return ERR_OPEN;
    }

    size_t res = fread(outData, numBytesPerItem, numItems, pFile);
    if (res < numItems) {
        FILE_READ_ERR(filePath, exitOnFailure);
        return ERR_READ;
    }

    if (fclose(pFile)) {
        FILE_CLOSE_ERR(filePath, exitOnFailure);
        return ERR_CLOSE;
    }

    return OK;
}

int runCommand(const char* command)
{
    // Execute the shell command (output into pipe)
    FILE *pipe = popen(command, "r");
    // Ignore output of the command; but consume it
    // so we don't get an error when closing the pipe.
    char buffer[1024];
    while (!feof(pipe) && !ferror(pipe)) {
        if (fgets(buffer, sizeof(buffer), pipe) == NULL)
            break;
        // printf("--> %s", buffer);  // Uncomment for debugging
    }
    // Get the exit code from the pipe; non-zero is an error:
    int exitCode = WEXITSTATUS(pclose(pipe));
    if (exitCode != COMMAND_SUCCESS) {
        SYS_WARN("Command '%s' failed with exit code '%d'.\n", command, exitCode);
    }
    return exitCode;
}

int64 getTimeInMs(void)
{
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);
    int64 seconds = spec.tv_sec;
    int64 nanoSeconds = spec.tv_nsec;
    int64 milliSeconds = seconds * 1000
                 + nanoSeconds / 1000000;
    return milliSeconds;
}

void sleepForMs(int64 delayInMs)
{
    const int64 NS_PER_MS = 1000 * 1000;
    const int64 NS_PER_SECOND = 1000000000;
    int64 delayNs = delayInMs * NS_PER_MS;
    int seconds = delayNs / NS_PER_SECOND;
    int nanoseconds = delayNs % NS_PER_SECOND;
    struct timespec reqDelay = {seconds, nanoseconds};
    nanosleep(&reqDelay, (struct timespec *) NULL);
}

// Avoid calling this macro nakedly to avoid side-effects.
#define MIN(a, b) ((a) < (b)) ? (a) : (b)

int64 int64_min(int64 a, int64 b) {
    // Safe to call macro here, since a and b have had a value copied to them already.
    return MIN(a, b);
}

// void Gpio_exportPin(GpioNum pin, char* header, GpioNum linuxPin)
// {
//     // Set pin to GPIO mode.
//     char pinString[4];
//     snprintf(pinString, 4, "%u", pin);
//     char command[MEDIUM_STRING_LEN];
//     snprintf(command, MEDIUM_STRING_LEN, "%s %s.%s gpio", GPIO_CONFIG_PIN_PATH, header, pinString);
//     int64 sleepMs = 35;
//     int maxTries = 3;
//     int ret = !OK;
//     for (int i = 0; i < maxTries && ((ret = runCommand(command)) == !OK); i++) {
//         LOG(LOG_LEVEL_DEBUG, "Trying again in %lld ms...\n", sleepMs);
//         sleepForMs(sleepMs);
//     }

//     // Export the pin.
//     char linuxPinString[4];
//     snprintf(linuxPinString, 4, "%u", linuxPin);
//     for (int i = 0; i < maxTries && ((ret = overwriteFile(GPIO_EXPORT_PATH, linuxPinString, false)) != OK); i++) {
//         LOG(LOG_LEVEL_DEBUG, "Trying again in %lld ms...\n", sleepMs);
//         sleepForMs(sleepMs);
//     }

//     if (ret == OK) {
//         LOG(LOG_LEVEL_DEBUG, "%s(%u, %s, %u) SUCCEEDED.\n\n", __func__, pin, header, linuxPin);
//     }
//     else {
//         LOG(LOG_LEVEL_DEBUG, "%s(%u, %s, %u) FAILED.\n\n", __func__, pin, header, linuxPin);
//     }
// }

// void Gpio_configIo(GpioNum linuxPin, bool isInput)
// {
//     char filePath[MEDIUM_STRING_LEN];
//     snprintf(filePath, MEDIUM_STRING_LEN, "%s%u/direction", GPIO_PIN_PATH_PREFIX, linuxPin);
//     int64 sleepMs = 35;
//     int maxTries = 10;
//     int ret = !OK;
//     for (int i = 0; i < maxTries && ((ret = overwriteFile(filePath, ((isInput) ? "in" : "out"), false)) != OK); i++) {
//         LOG(LOG_LEVEL_DEBUG, "Trying again in %lld ms...\n", sleepMs);
//         sleepForMs(sleepMs);
//     }
//     if (ret == OK) {
//         LOG(LOG_LEVEL_DEBUG, "%s(%u, %u) SUCCEEDED.\n\n", __func__, linuxPin, isInput);
//     }
//     else {
//         LOG(LOG_LEVEL_DEBUG, "%s(%u, %u) FAILED.\n\n", __func__, linuxPin, isInput);
//     }

// }

// void Gpio_initPin(GpioNum pin, char* header, GpioNum linuxPin, bool isInput)
// {
//     Gpio_exportPin(pin, header, linuxPin);

//     LOG(LOG_LEVEL_DEBUG, "Calling Gpio_configIo()...\n");
//     Gpio_configIo(linuxPin, isInput);
// }

// int Gpio_readInput(GpioNum linuxPin)
// {
//     char filePath[MEDIUM_STRING_LEN];
//     snprintf(filePath, MEDIUM_STRING_LEN, "%s%u/value", GPIO_PIN_PATH_PREFIX, linuxPin);

//     char valueString[2];
//     int res = readFile(filePath, (void*) valueString, sizeof(char), 1, false);
//     if (res == OK) {
//         // TODO: atoi also returns zero if the conversion failed! In this case, ideally would use a different method.
//         return atoi(valueString);
//     }
//     else {
//         return GPIO_READ_ERR;
//     }
// }