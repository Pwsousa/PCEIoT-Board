#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "luminosity.h"

int main() {
    stdio_init_all();

    // Inicializa I²C
    i2c_init(I2C_PORT, 100 * 1000);  
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    opt4001_init();
    sleep_ms(500);

    printf("Iniciando leitura do OPT4001...\n");

    while (1) {
        float lux = opt4001_read_lux();
        printf("Luminosidade: %.2f lux\n", lux);
        sleep_ms(1000);
    }
}