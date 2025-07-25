#include <stdio.h>
#include "opt4001.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"


int opt4001_init(OPT4001 *sensor, i2c_inst_t *i2c, uint8_t address) {
    sensor->i2c = i2c;
    sensor->address = address;
    sensor->offset = 0.0f;

    
    i2c_init(i2c, 100 * 1000); 


    gpio_set_function(4, GPIO_FUNC_I2C); 
    gpio_set_function(5, GPIO_FUNC_I2C);  
    gpio_pull_up(4);
    gpio_pull_up(5);

    return 0;
}

int opt4001_write_register(OPT4001 *sensor, uint8_t reg, uint16_t value) {
    uint8_t buf[3] = { reg, (uint8_t)(value >> 8), (uint8_t)(value & 0xFF) };
    return write(sensor->i2c, buf, 3) == 3 ? 0 : -1;
}

int opt4001_read_register(OPT4001 *sensor, uint8_t reg, uint16_t *value) {
    if (write(sensor->i2c, &reg, 1) != 1) return -1;

    uint8_t buf[2];
    if (read(sensor->i2c, buf, 2) != 2) return -1;

    *value = ((uint16_t)buf[0] << 8) | buf[1];
    return 0;
}

int opt4001_read_raw(OPT4001 *sensor, uint32_t *raw, uint8_t *exp) {
    uint8_t start_config[] = { 0x0A, 0x32, 0x28 };
    write(sensor->i2c, start_config, 3);
    usleep(200000);

    uint8_t reg = 0x00;
    write(sensor->i2c, &reg, 1);

    uint8_t data[2];
    read(sensor->i2c, data, 2);

    *exp = data[0] >> 4;
    *raw = ((data[0] & 0x0F) << 16) | (data[1] << 8);

    reg = 0x01;
    write(sensor->i2c, &reg, 1);
    read(sensor->i2c, data, 2);

    *raw |= data[0];
    return 0;
}

float opt4001_read_lux(OPT4001 *sensor) {
    uint32_t raw;
    uint8_t exp;
    if (opt4001_read_raw(sensor, &raw, &exp) != 0) return -1.0f;

    float lux = (raw << exp) * 0.0004375f + sensor->offset;
    return lux;
}

bool opt4001_is_connected(OPT4001 *sensor) {
    uint16_t value;
    return opt4001_read_register(sensor, 0x01, &value) == 0;
}

int opt4001_get_device_id(OPT4001 *sensor, uint16_t *id) {
    return opt4001_read_register(sensor, 0x7E, id);
}

int opt4001_get_status(OPT4001 *sensor, uint16_t *status) {
    return opt4001_read_register(sensor, 0x02, status);
}

int opt4001_reset(OPT4001 *sensor) {
    return opt4001_write_register(sensor, 0x0A, 0xC000);
}

void opt4001_set_offset(OPT4001 *sensor, float offset) {
    sensor->offset = offset;
}
