#include <stdio.h>

#include "AHT10.h"
#include "ch32v003fun.h"

AHT10_Dev aht10;

int main() {
    SystemInit();

    // Initialize sensor
    if (!AHT10_begin(&aht10)) {
        printf("Sensor initialization failed!\r\n");
        while (1);
    }

    while (1) {
        if (AHT10_read(&aht10)) {
            printf("Temperature: %d.%d°C\r\n", aht10.temperature_x10 / 10,
                   aht10.temperature_x10 < 0 ? -aht10.temperature_x10 % 10
                                             : aht10.temperature_x10 % 10);

            printf("Humidity: %d.%d%%\r\n", aht10.humidity_x10 / 10,
                   aht10.humidity_x10 % 10);
        }
        Delay_Ms(2000);  // Read every 2 seconds
    }
}
