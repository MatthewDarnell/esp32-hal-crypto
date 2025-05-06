//
// Created by Matthew Darnell on 5/4/25.
//

#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"

void randombytes(uint8_t* random_bytes, size_t bytes) {
/*
    https://www.espressif.com/sites/default/files/documentation/esp32_technical_reference_manual_en.pdf
    Section 25.5

    Note: When using the random number generator, make sure at least either the SAR ADC or high-speed ADC is enabled.
    Otherwise, pseudo-random numbers will be returned.

    (High-Speed ADC is enabled automatically when the Wi-Fi or Bluetooth modules is enabled.)
*/
    const uint32_t RNG = 0x3FF75144;

    size_t read = 0;
    uint32_t random = 0;
    while (read <= bytes - 4) {
        random = REG_READ(RNG);
        random_bytes[read++] = random >> 3 & 0xFF;
        random_bytes[read++] = random >> 2 & 0xFF;
        random_bytes[read++] = random >> 1 & 0xFF;
        random_bytes[read++] = random & 0xFF;
    }
    if (read < bytes) {
        random = REG_READ(RNG);
        random_bytes[read++] = random >> 3 & 0xFF;
        if (read < bytes) {
            random_bytes[read++] = random >> 2 & 0xFF;
        }
        if (read < bytes) {
            random_bytes[read++] = random >> 1 & 0xFF;
        }
        if (read < bytes) {
            random_bytes[read] = random & 0xFF;
        }
    }
}