#include <stdio.h>

#include "adc.h"
#include "utils.h"
#include "gpio.h"
#include "i2c.h"
#include "return_val.h"
#include "log.h"
#include "digit_display.h"
#include "adc_buffer.h"

int main(int argc, char* args[]) {
    initLogLevel();
    printf("Hello embedded world!\n");

    AdcBuffer buffer;
    AdcBuffer* pBuffer = &buffer;
    AdcBuffer_init(pBuffer, 10);
    for (uint64 i = 0; i < pBuffer->capacity - 1; i++) {
        AdcBuffer_add(pBuffer, i);
    }
    AdcBuffer_printAll(pBuffer);
    // Add to already full buffer. Should be no problem, just overwrite first element.

    for (uint64 i = 0; i < 15; i++) {
        AdcBuffer_add(pBuffer, pBuffer->size + i);
        printf("Added %u.\n", (adc_in) (pBuffer->size + i));
        AdcBuffer_printAll(pBuffer);
    }

    // Try increasing buffer size.
    AdcBuffer_resize(pBuffer, 1);
    printf("Resized to 1.\n");
    AdcBuffer_printAll(pBuffer);
    AdcBuffer_add(pBuffer, 77);
    printf("Added 77.\n");
    AdcBuffer_printAll(pBuffer);
    AdcBuffer_add(pBuffer, 55);
    printf("Added 55.\n");
    AdcBuffer_printAll(pBuffer);

    // Try decreasing buffer size.
    AdcBuffer_resize(pBuffer, 10);
    printf("Resized to 10.\n");
    AdcBuffer_printAll(pBuffer);

    // Try "resizing" but size is the same as current size.
    AdcBuffer_resize(pBuffer, 9);
    printf("Resized to 9.\n");
    AdcBuffer_printAll(pBuffer);

    AdcBuffer_cleanup(pBuffer);


    // // Read from analog input once per second.
    // // AIN1 Should be where my photoresistor stuff is connected to.
    // // AIN0 is the zen cape's potentiometer
    // while (1) {
    //     adc_in input0 = ADC_MAX_IN * 2;
    //     int res = Adc_readInput(0, &input0);
    //     if (res != OK) {
    //         LOG(LOG_LEVEL_WARN, "Uh oh.\n");
    //     }
    //     volt voltage = Adc_convertToVolts(input0);
    //     printf("AIN0\n");
    //     printf("ADC input = %u\n", input0);
    //     printf("In volts = %f\n", voltage);
    //     printf("\n");

    //     // Sleep for 1 second.
    //     sleepForMs(NUM_MS_PER_S);

    //     adc_in input1 = ADC_MAX_IN * 2;
    //     int res1 = Adc_readInput(1, &input1);
    //     if (res1 != OK) {
    //         LOG(LOG_LEVEL_WARN, "Uh oh.\n");
    //     }
    //     volt voltage1 = Adc_convertToVolts(input1);
    //     printf("AIN1\n");
    //     printf("ADC input = %u\n", input1);
    //     printf("In volts = %f\n", voltage1);
    //     printf("\n");

    //     // Sleep for 1 second.
    //     sleepForMs(NUM_MS_PER_S);
    // }

    // DigitDisplay_init();

    return 0;
}