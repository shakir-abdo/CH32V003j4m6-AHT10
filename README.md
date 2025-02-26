
# AHT10 Temperature & Humidity Sensor Library

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Platform: CH32V003](https://img.shields.io/badge/Platform-CH32V003-blue.svg)](http://www.wch-ic.com/products/CH32V003.html)

A lightweight and efficient library for the AHT10 temperature and humidity sensor, specifically designed for the CH32V003 microcontroller.

## Features

- Simple and user-friendly API
- Fixed-point arithmetic for efficient operation
- I2C communication at 400kHz
- One decimal place precision for both temperature and humidity
- Timeout protection for I2C operations
- Minimal RAM usage

## Hardware Setup

Connect the AHT10 sensor to the CH32V003 as follows:

- SDA → PC1
- SCL → PC2
- VCC → 3.3V
- GND → GND

## Installation

1. Create a folder named `AHT10` in your project's `lib` directory
2. Copy `AHT10.c` and `AHT10.h` into this folder
3. Include the library in your project: `#include "AHT10.h"`

## Usage

```c
#include "ch32v003fun.h"
#include "AHT10.h"
#include <stdio.h>

AHT10_Dev aht10;

int main() {
    SystemInit();

    // Initialize sensor
    if (!AHT10_begin(&aht10)) {
        printf("Sensor initialization failed!\r\n");
        while(1);
    }

    while(1) {
        if (AHT10_read(&aht10)) {
            printf("Temperature: %d.%d°C\r\n",
                   aht10.temperature_x10/10,
                   aht10.temperature_x10 < 0 ? -aht10.temperature_x10%10 : aht10.temperature_x10%10);

            printf("Humidity: %d.%d%%\r\n",
                   aht10.humidity_x10/10,
                   aht10.humidity_x10%10);
        }
        Delay_Ms(2000);  // Read every 2 seconds
    }
}
```

## API Reference

### Functions

- `bool AHT10_begin(AHT10_Dev* dev)`
  - Initializes the sensor and I2C communication
  - Returns `true` if initialization successful

- `bool AHT10_read(AHT10_Dev* dev)`
  - Reads temperature and humidity from sensor
  - Returns `true` if read successful

- `void AHT10_reset(void)`
  - Performs a soft reset of the sensor

### Data Structure

```c
typedef struct {
    int16_t temperature_x10; // Temperature × 10 (e.g., 234 means 23.4°C)
    int16_t humidity_x10;    // Humidity × 10 (e.g., 450 means 45.0%)
} AHT10_Dev;
```

## Specifications

- Temperature Range: -40°C to +85°C
- Temperature Accuracy: ±0.3°C
- Humidity Range: 0% to 100% RH
- Humidity Accuracy: ±2% RH
- I2C Address: 0x38
- Operating Voltage: 3.3V

## Platform Compatibility

- Microcontroller: CH32V003
- Framework: ch32v003fun

## License

MIT License

## Author

- **Shakir Abdo**
- Email: shicolare1@gmail.com

## Acknowledgements

- Thanks to the ch32v003fun framework developers

## Version History

- 1.0.0 (2024-02-17)
  - Initial release
  - Basic temperature and humidity reading functionality
  - Fixed-point arithmetic implementation

## Future Improvements

- Add power management features
- Add CRC error checking
- Add multi-device support
- Add different precision modes support

