#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define I2C_PORT i2c0
#define SDA_PIN 8
#define SCL_PIN 5

#define OPT4001_ADDR 0x44

#define REG_CONFIG 0x00
#define REG_RESULT 0x02

void opt4001_init();
uint16_t opt4001_read_raw();
float opt4001_read_lux();