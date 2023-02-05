#include <pthread.h>
#include <stdbool.h>

#include "light_sampler.h"
#include "light_meter.h"
#include "shutdown.h"
#include "log.h"
#include "utils.h"
#include "periodTimer.h"

static bool initialized = false;
static pthread_t thread;

void LightSampler_init(AdcBuffer* pBuffer)
{
    if (!initialized) {
        int res = pthread_create(&thread, NULL, LightSampler_run, pBuffer);
        if (res != 0) {
            SYS_DIE("pthread_create failed.\n");
        }
        initialized = true;
    }
}

void* LightSampler_run(void* args)
{
    AdcBuffer* pBuffer = (AdcBuffer*) args;
    while (!isShutdownRequested()) {
        adc_in brightness = LightMeter_read();
        AdcBuffer_add(pBuffer, brightness);
        Period_markEvent(PERIOD_EVENT_SAMPLE_LIGHT);

        sleepForMs(1);
    }

    return NULL;
}

void LightSampler_waitForShutdown(void)
{
    int res = pthread_join(thread, NULL);
    if (res != 0) {
        SYS_WARN("pthread_join failed.\n");
    }
    initialized = false;
}