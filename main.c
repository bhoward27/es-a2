// Program entry point -- all the magic starts here.
#include <stdio.h>
#include <stdatomic.h>

#include "adc.h"
#include "utils.h"
#include "gpio.h"
#include "i2c.h"
#include "return_val.h"
#include "log.h"
#include "digit_display.h"
#include "adc_buffer.h"
#include "periodTimer.h"
#include "console.h"
#include "light_sampler.h"
#include "shutdown.h"
#include "udp_server.h"

int main(int argc, char* args[]) {
    initLogLevel();
    AdcBuffer buffer;
    AdcBuffer_init(&buffer, 1000);
    Period_init();
    LightSampler_init(&buffer);
    DigitDisplay_init(&buffer);
    Console_init(&buffer);
    UdpServer_init(&buffer);

    // TODO: Remove.
    // sleepForMs(10 * NUM_MS_PER_S);
    // requestShutdown();

    UdpServer_waitForShutdown();
    Console_waitForShutdown();
    DigitDisplay_waitForShutdown();
    LightSampler_waitForShutdown();
    Period_cleanup();
    AdcBuffer_cleanup(&buffer);

    return 0;
}