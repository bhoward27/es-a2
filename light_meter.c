#include <stdbool.h>

#include "light_meter.h"

static bool initialized = false;

void LightMeter_init(void)
{
    if (!initialized) {
        // TODO: Do init procedure.

        initialized = true;
    }
}


void LightMeter_cleanup(void)
{
    // TODO: Do cleanup.


    initialized = false;
}

volt LightMeter_read(void)
{

}
