#include "AHT10.h"

#include <stdio.h>

#define AHT10_ADDR 0x38
#define AHT10_INIT_CMD 0xE1
#define AHT10_MEASURE_CMD 0xAC
#define AHT10_RESET_CMD 0xBA

// Private function prototypes
static bool i2c_write(uint8_t* data, uint8_t len);
static bool i2c_read(uint8_t* data, uint8_t len);

bool AHT10_begin(AHT10_Dev* dev) {
    // Initialize I2C
    RCC->APB1PCENR |= RCC_APB1Periph_I2C1;
    RCC->APB2PCENR |= RCC_APB2Periph_GPIOC;

    // Reset I2C
    RCC->APB1PRSTR |= RCC_APB1Periph_I2C1;
    RCC->APB1PRSTR &= ~RCC_APB1Periph_I2C1;

    // Configure I2C pins (PC1=SDA, PC2=SCL)
    GPIOC->CFGLR &= ~(0xFF << (4 * 1));
    GPIOC->CFGLR |= (GPIO_CFGLR_OUT_10Mhz_AF_OD << (4 * 1)) |
                    (GPIO_CFGLR_OUT_10Mhz_AF_OD << (4 * 2));

    // Configure I2C timing for 400kHz
    I2C1->CTLR2 = 24;    // Input clock 24MHz
    I2C1->CKCFGR = 120;  // 400kHz @ 24MHz

    // Enable I2C
    I2C1->CTLR1 |= I2C_CTLR1_PE;

    // Initialize sensor data
    dev->temperature_x10 = 0;
    dev->humidity_x10 = 0;

    // Reset sensor
    AHT10_reset();
    Delay_Ms(20);

    // Initialize sensor
    uint8_t cmd[3] = {AHT10_INIT_CMD, 0x08, 0x00};
    if (!i2c_write(cmd, 3)) return false;
    Delay_Ms(10);

    return true;
}

void AHT10_reset(void) {
    uint8_t cmd = AHT10_RESET_CMD;
    i2c_write(&cmd, 1);
    Delay_Ms(20);
}

bool AHT10_read(AHT10_Dev* dev) {
    uint8_t cmd[3] = {AHT10_MEASURE_CMD, 0x33, 0x00};
    uint8_t data[6];

    // Send measurement command
    if (!i2c_write(cmd, 3)) return false;
    Delay_Ms(80);  // Wait for measurement

    // Read data
    if (!i2c_read(data, 6)) return false;

    // Check busy bit
    if (data[0] & 0x80) return false;

    // Calculate humidity
    uint32_t humidity_raw =
        ((uint32_t)data[1] << 16) | ((uint32_t)data[2] << 8) | data[3];
    humidity_raw = humidity_raw >> 4;

    // Calculate temperature
    uint32_t temp_raw =
        (((uint32_t)data[3] & 0x0F) << 16) | ((uint32_t)data[4] << 8) | data[5];

    // Convert to fixed-point values (×10 for one decimal place)
    dev->humidity_x10 = (int16_t)((humidity_raw * 1000.0) / 1048576.0);
    dev->temperature_x10 = (int16_t)(((temp_raw * 2000.0) / 1048576.0) - 500.0);

    return true;
}

// Private I2C helper functions
static bool i2c_write(uint8_t* data, uint8_t len) {
    uint32_t timeout = 10000;

    while (I2C1->STAR2 & I2C_STAR2_BUSY) {
        if (--timeout == 0) return false;
    }

    I2C1->CTLR1 |= I2C_CTLR1_START;
    timeout = 10000;
    while (!(I2C1->STAR1 & I2C_STAR1_SB)) {
        if (--timeout == 0) return false;
    }

    I2C1->DATAR = AHT10_ADDR << 1;
    timeout = 10000;
    while (!(I2C1->STAR1 & I2C_STAR1_ADDR)) {
        if (--timeout == 0) return false;
    }
    (void)I2C1->STAR2;

    for (uint8_t i = 0; i < len; i++) {
        I2C1->DATAR = data[i];
        timeout = 10000;
        while (!(I2C1->STAR1 & I2C_STAR1_BTF)) {
            if (--timeout == 0) return false;
        }
    }

    I2C1->CTLR1 |= I2C_CTLR1_STOP;
    return true;
}

static bool i2c_read(uint8_t* data, uint8_t len) {
    uint32_t timeout = 10000;

    I2C1->CTLR1 |= I2C_CTLR1_START;
    timeout = 10000;
    while (!(I2C1->STAR1 & I2C_STAR1_SB)) {
        if (--timeout == 0) return false;
    }

    I2C1->DATAR = (AHT10_ADDR << 1) | 0x01;
    timeout = 10000;
    while (!(I2C1->STAR1 & I2C_STAR1_ADDR)) {
        if (--timeout == 0) return false;
    }
    (void)I2C1->STAR2;

    for (uint8_t i = 0; i < len; i++) {
        if (i == len - 1) {
            I2C1->CTLR1 &= ~I2C_CTLR1_ACK;
        } else {
            I2C1->CTLR1 |= I2C_CTLR1_ACK;
        }

        timeout = 10000;
        while (!(I2C1->STAR1 & I2C_STAR1_RXNE)) {
            if (--timeout == 0) return false;
        }
        data[i] = I2C1->DATAR;
    }

    I2C1->CTLR1 |= I2C_CTLR1_STOP;
    return true;
}
