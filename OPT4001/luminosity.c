// Inicializa o sensor OPT4001 (modo contínuo)
#include <stdio.h>
#include "pico/stdlib.h"
#include "luminosity.h"
    
void opt4001_init() {
    uint8_t config_data[3];
    config_data[0] = REG_CONFIG;
    // Configuração genérica: habilita conversão contínua
    config_data[1] = 0xCE;
    config_data[2] = 0x10;
    i2c_write_blocking(I2C_PORT, OPT4001_ADDR, config_data, 3, false);
}

// Lê o valor bruto (16 bits) do sensor
uint16_t opt4001_read_raw() {
    uint8_t reg = REG_RESULT;
    uint8_t buf[2];
    i2c_write_blocking(I2C_PORT, OPT4001_ADDR, &reg, 1, true);  // Seleciona registrador
    i2c_read_blocking(I2C_PORT, OPT4001_ADDR, buf, 2, false);   // Lê 2 bytes
    return (buf[0] << 8) | buf[1];
}

// Converte o valor bruto em lux (aproximado)
float opt4001_read_lux() {
    uint16_t raw = opt4001_read_raw();
    return raw * 0.01f;  // Escala simples (~0.01 lux por bit)
}