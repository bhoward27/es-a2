#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "gpio.h"
#include "utils.h"
#include "log.h"
#include "return_val.h"

int Gpio_precheckSetPinMode(const char* header, const char* pin, const char* mode, size_t maxCharsToCompare)
{
    /**
     * This function sets the pin's mode to mode only if the pin is not already set to that mode.
     *
     * It's recommended that mode be an array of size GPIO_MAX_MODE_LEN for more reliable string comparison internal to
     * to this function.
     */

    assert(mode);

    char currentMode[GPIO_MAX_MODE_LEN];
    int queryRes = Gpio_queryPinMode(header, pin, currentMode, GPIO_MAX_MODE_LEN);
    if (queryRes != COMMAND_SUCCESS) {
        return queryRes;
    }

    // Unintended consequence of passing min(...) to strncmp() can be illustrated in an example:
    // Suppose currentMode = "abc" and mode = "abz", and min(...) = 2. Then strncmp will incorrectly tell us that
    // currentMode == mode.
    // However, this unintended consequence is better than getting a segmentation fault, which could happen if passed
    // a non-null-terminated string. This is why it's recommended that mode be of size
    // GPIO_MAX_MODE_LEN = maxCharsToCompare.
    int isDifferentMode = strncmp(mode, currentMode, int64_min(GPIO_MAX_MODE_LEN, maxCharsToCompare));
    if (isDifferentMode) {
        int setRes = Gpio_setPinMode(header, pin, mode);
        if (setRes == COMMAND_SUCCESS) {
            LOG(LOG_LEVEL_DEBUG,
                "Changed GPIO mode for %s.%s from '%s' to '%s'.\n", header, pin, currentMode, mode);
        }
        return setRes;
    }

    return COMMAND_SUCCESS;
}

int Gpio_setPinMode(const char* header, const char* pin, const char* mode)
{
    assert(header);
    assert(mode);
    assert(pin);

    char command[MEDIUM_STRING_LEN];
    snprintf(command, MEDIUM_STRING_LEN, "%s %s.%s %s", GPIO_CONFIG_PIN_PATH, header, pin, mode);
    int res = runCommand(command);

    return res;
}

int Gpio_queryPinMode(const char* header, const char* pin, char* outMode, size_t maxModeLen)
{
    assert(header);
    assert(pin);
    assert(outMode);

    char command[MEDIUM_STRING_LEN];
    // Citation: How to suppress blank lines in awk taken from here:
    // https://stackoverflow.com/questions/10347653/awk-remove-blank-lines
    snprintf(command, MEDIUM_STRING_LEN, "%s -q %s.%s | awk '!/^$/ {print $6}'", GPIO_CONFIG_PIN_PATH, header, pin);

    char output[MEDIUM_STRING_LEN];
    FILE* pipe = popen(command, "r");
    while (!feof(pipe) && !ferror(pipe)) {
        if (!fgets(output, sizeof(output), pipe)) {
            break;
        }
    }

    int exitCode = WEXITSTATUS(pclose(pipe));
    if (exitCode != COMMAND_SUCCESS) {
        SYS_WARN("Command '%s' failed with exit code '%d'.\n", command, exitCode);
    }
    else {
        size_t maxLen = int64_min(maxModeLen, MEDIUM_STRING_LEN);
        size_t len = strnlen(output, maxLen - 1);

        // Want everything in output except the \n, which will the be the last character.
        strncpy(outMode, output, len - 1);
        outMode[len - 1] = '\0';
    }

    return exitCode;
}