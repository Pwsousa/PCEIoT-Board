#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "opt4001.h"

#define I2C_PORT i2c0
#define OPT4001_ADDR 0x44

int main() {
    stdio_init_all();
    sleep_ms(2000);  

    printf("Iniciando testes com sensor OPT4001...\n");

    OPT4001 sensor;
    if (opt4001_init(&sensor, I2C_PORT, OPT4001_ADDR) != 0) {
        printf("Falha ao inicializar sensor!\n");
        return 1;
    }

    
    if (!opt4001_is_connected(&sensor)) {
        printf("Sensor OPT4001 não está conectado!\n");
        return 1;
    } else {
        printf("Sensor conectado com sucesso.\n");
    }

    
    uint16_t id;
    if (opt4001_get_device_id(&sensor, &id) == 0) {
        printf("ID do dispositivo: 0x%04X\n", id);
    } else {
        printf("Falha ao ler ID do dispositivo.\n");
    }

   
    uint16_t status;
    if (opt4001_get_status(&sensor, &status) == 0) {
        printf("Status do sensor: 0x%04X\n", status);
    } else {
        printf("Falha ao ler status do sensor.\n");
    }

   
    printf("Iniciando leitura contínua de lux:\n");
    for (int i = 0; i < 10; i++) {
        float lux = opt4001_read_lux(&sensor);
        if (lux < 0) {
            printf("Erro na leitura de lux!\n");
        } else {
            printf("Lux[%d]: %.4f lx\n", i, lux);
        }
        sleep_ms(1000);
    }

    // Teste de reset
    if (opt4001_reset(&sensor) == 0) {
        printf("Sensor resetado com sucesso.\n");
    } else {
        printf("Erro ao resetar o sensor.\n");
    }

    printf("Testes finalizados.\n");

    while (true) {
        tight_loop_contents(); 
    }

    return 0;
}
