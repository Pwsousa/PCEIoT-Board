#include <stdio.h>
#include "pico/stdlib.h"
#include "sd_card_info.h"

int main() {
    stdio_init_all();
    printf("\nMonitor de Cartao SD\n");
    printf("===================\n");
    
    sd_init();
    
    while(1) {
        if(sd_is_present()) {
            printf("Cartao detectado!\n");
            float cap_mb = sd_get_capacity_mb();
            printf("Capacidade: %.2f MB\n", cap_mb);
        } else {
            printf("Insira um cartao SD...\n");
        }
        
        sleep_ms(2000);
    }
    
    return 0;
}