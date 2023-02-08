#include <stdbool.h>
#include <pthread.h>
#include <stdio.h>

#include "console.h"
#include "shutdown.h"
#include "utils.h"
#include "log.h"
#include "potentiometer.h"
#include "adc_stats.h"
#include "periodTimer.h"

static bool initialized = false;
static pthread_t thread;

void Console_init(AdcBuffer* pBuffer)
{
    if (!initialized) {
        int res = pthread_create(&thread, NULL, Console_run, pBuffer);
        if (res != 0) {
            SYS_DIE("pthread_create failed.\n");
        }
        initialized = true;
    }
}

void Console_waitForShutdown(void)
{
    int res = pthread_join(thread, NULL);
    if (res != 0) {
        SYS_WARN("pthread_join failed.\n");
    }
    initialized = false;
}

// Run this in a thread.
void* Console_run(void* args)
{
    AdcBuffer* pBuffer = (AdcBuffer*) args;
    while (!isShutdownRequested()) {
        adc_in potInput = Potentiometer_read();
        uint64 newSize = (potInput == 0) ? 1 : potInput;
        AdcBuffer_resize(pBuffer, newSize);

        uint64 numSamples = 0;
        adc_in* samples = AdcBuffer_getSamples(pBuffer, &numSamples);
        double meanBrightness = AdcBuffer_getCurrentMean(pBuffer);
        uint64 numDips =  AdcStats_dips(samples, numSamples, meanBrightness);
        double meanBrightnessV = Adc_convertToVolts(meanBrightness);

        Period_statistics_t jitterStats;
        Period_getStatisticsAndClear(PERIOD_EVENT_SAMPLE_LIGHT, &jitterStats);

        printf(
            "Samples/s = %d\t\tPot Value = %hu\thistory size = %llu\tavg = %.3f\tdips = %llu\tSampling[ %.3f, %.3f ] avg %.3f/%d\n",
            jitterStats.numSamples,
            potInput,
            numSamples,
            meanBrightnessV,
            numDips,
            jitterStats.minPeriodInMs,
            jitterStats.maxPeriodInMs,
            jitterStats.avgPeriodInMs,
            jitterStats.numSamples
        );

        // Print every two-hundredth sample
        const uint64 step = 200;
        for (uint64 i = 0; i < numSamples; i += step) {
            printf("%.3f ", Adc_convertToVolts(samples[i]));
        }
        printf("\n");

        free(samples);

        sleepForMs(NUM_MS_PER_S);
    }

    return NULL;
}

