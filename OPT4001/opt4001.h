#ifndef OPT4001_H
#define OPT4001_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    int i2c_fd;        // File descriptor do /dev/i2c-*
    uint8_t address;   // Endereço do dispositivo (0x44, por ex.)
    float offset;      // Offset opcional para calibração
} OPT4001;


int opt4001_init(OPT4001 *sensor, const char *i2c_path, uint8_t address);


float opt4001_read_lux(OPT4001 *sensor);


int opt4001_read_raw(OPT4001 *sensor, uint32_t *raw, uint8_t *exp);


bool opt4001_is_connected(OPT4001 *sensor);


int opt4001_get_device_id(OPT4001 *sensor, uint16_t *id);


int opt4001_get_status(OPT4001 *sensor, uint16_t *status);


int opt4001_write_register(OPT4001 *sensor, uint8_t reg, uint16_t value);


int opt4001_read_register(OPT4001 *sensor, uint8_t reg, uint16_t *value);


int opt4001_reset(OPT4001 *sensor);


void opt4001_set_offset(OPT4001 *sensor, float offset);


void opt4001_close(OPT4001 *sensor);

#endif // OPT4001_H
