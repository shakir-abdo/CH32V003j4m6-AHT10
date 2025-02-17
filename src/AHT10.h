#ifndef AHT10_H
#define AHT10_H

#include <stdbool.h>

#include "ch32v003fun.h"

// Structure to hold sensor data
typedef struct {
    int16_t temperature_x10;  // Temperature × 10 (e.g., 234 means 23.4°C)
    int16_t humidity_x10;     // Humidity × 10 (e.g., 450 means 45.0%)
} AHT10_Dev;

// Function prototypes
bool AHT10_begin(AHT10_Dev* dev);
bool AHT10_read(AHT10_Dev* dev);
void AHT10_reset(void);

#endif
