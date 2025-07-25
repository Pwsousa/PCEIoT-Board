#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define I2C_PORT i2c0
#define SDA_PIN 8
#define SCL_PIN 9

#define OPT4001_ADDR 0x44

#define REG_CONFIG 0x00
#define REG_RESULT 0x02

void opt4001_init() {
    uint8_t config_data[3];
    config_data[0] = REG_CONFIG; 
    // Configuração básica: modo contínuo (bits dependem do datasheet)
    config_data[1] = 0xCE;  // Parte alta (exemplo, habilita conversão contínua)
    config_data[2] = 0x10;  // Parte baixa (ajuste de tempo/integr.)
    i2c_write_blocking(I2C_PORT, OPT4001_ADDR, config_data, 3, false);
}

uint16_t opt4001_read_raw() {
    uint8_t reg = REG_RESULT;
    uint8_t buf[2];
    i2c_write_blocking(I2C_PORT, OPT4001_ADDR, &reg, 1, true);  // Seleciona registrador
    i2c_read_blocking(I2C_PORT, OPT4001_ADDR, buf, 2, false);   // Lê 2 bytes
    return (buf[0] << 8) | buf[1];
}

float opt4001_read_lux() {
    uint16_t raw = opt4001_read_raw();
    // Conversão básica: cada bit ≈ 0.01 lux (ajustar conforme datasheet/config)
    return raw * 0.01f;
}

int main() {
    stdio_init_all();

    // Inicializa I2C
    i2c_init(I2C_PORT, 100 * 1000);  // 100kHz
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    opt4001_init();
    sleep_ms(500);

    while (1) {
        float lux = opt4001_read_lux();
        printf("Luminosidade: %.2f lux\n", lux);
        sleep_ms(1000);
    }
}