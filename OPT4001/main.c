#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define I2C_PORT i2c0
#define SDA_PIN 8
#define SCL_PIN 5

int main() {
    stdio_init_all();

    i2c_init(I2C_PORT, 100000);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    printf("Iniciando sensor OPT4001...\n");

    // Configura os limites e habilita interrupções
    configurar_limites_opt4001();
    opt4001_enable_threshold_interrupt();

    sleep_ms(100);

    while (true) {
        // Leitura precisa
        float lux = opt4001_read_lux_precise();
        printf("Luminosidade precisa: %.2f lux\n", lux);

        // Leitura one-shot
        float lux_one_shot = opt4001_read_lux_one_shot();
        if (lux_one_shot >= 0)
            printf("Luminosidade one-shot: %.2f lux\n", lux_one_shot);
        else
            printf("Falha na leitura one-shot\n");

        // Verifica se está fora dos limites
        if (opt4001_is_above_threshold()) {
            printf("Acima do limite superior!\n");
        } else if (opt4001_is_below_threshold()) {
            printf("Abaixo do limite inferior!\n");
        } else {
            printf("Dentro dos limites normais.\n");
        }

        // Lê amostras do FIFO
        float fifo_samples[8];
        int samples = opt4001_read_fifo_all(fifo_samples, 8);
        if (samples > 0) {
            printf("Amostras FIFO (%d): ", samples);
            for (int i = 0; i < samples; i++) {
                printf("%.2f ", fifo_samples[i]);
            }
            printf("\n");
        }

        printf("-------------------------\n");

        sleep_ms(2000);
    }

    return 0;
}
