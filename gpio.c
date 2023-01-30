#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "gpio.h"
#include "utils.h"
#include "log.h"
#include "return_val.h"

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

int Gpio_queryPinMode(const char* header, const char* pin, char* outMode, size_t modeMaxLen)
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
        size_t maxLen = (modeMaxLen < MEDIUM_STRING_LEN) ? modeMaxLen : MEDIUM_STRING_LEN;
        size_t len = strnlen(output, maxLen - 1);

        // Want everything in output except the \n, which will the be the last character.
        strncpy(outMode, output, len - 1);
        outMode[len - 1] = '\0';
    }

    return exitCode;
}