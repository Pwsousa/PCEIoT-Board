#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define I2C_PORT i2c0
#define SDA_PIN 8
#define SCL_PIN 9

#define OPT4001_ADDR 0x44

#define REG_RESULT_MSB 0x00
#define REG_RESULT_LSB 0x01
#define REG_EXPONENT   0x02

#define REG_CONFIGURATION   0x03
#define REG_COUNTER         0x10

#define REG_THRESHOLD_H_RESULT     0x09
#define REG_THRESHOLD_H_EXPONENT   0x0A
#define REG_THRESHOLD_L_RESULT     0x0B
#define REG_THRESHOLD_L_EXPONENT   0x0C
#define REG_FLAG_H                 0x0E
#define REG_FLAG_L                 0x0F
#define REG_INTERRUPT_CONFIG  0x07
#define REG_INTERRUPT_DIR     0x08


void opt4001_init();
uint16_t opt4001_read_raw();
float opt4001_read_lux();

float opt4001_read_lux_precise();
void opt4001_trigger_one_shot();
uint8_t opt4001_read_counter();
float opt4001_read_lux_one_shot();
void opt4001_set_thresholds();
bool opt4001_is_above_threshold();
bool opt4001_is_below_threshold();
void configurar_limites_opt4001();

uint8_t opt4001_crc8(const uint8_t *data, size_t len);

#define REG_INTERRUPT_DIR     0x0E
#define REG_INTERRUPT_CONFIG  0x0F
#define REG_FIFO_CONFIG       0x10

void opt4001_enable_fifo_interrupt(uint8_t fifo_level);

#define REG_RESULT     0x02
#define REG_EXPONENT   0x04
#define REG_FIFO_COUNT 0x11

