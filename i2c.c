#include <stdbool.h>

#include "i2c.h"

static bool initialized = false;

void I2c_init(void)
{
    if (!initialized) {
        // TODO: initialize.
    }
}

void I2c_cleanup(void)
{
    // TODO: clean up.

    initialized = false;
}



// TODO: i2c_write