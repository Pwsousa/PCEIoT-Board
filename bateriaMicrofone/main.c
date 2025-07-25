#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "Microfone.h"
#include "bateria.h" // Inclui sua biblioteca da bateria

#ifndef SAMPLE_RATE
#define SAMPLE_RATE 10000
#endif

// Intervalo para leitura da bateria em milissegundos
#define INTERVALO_BATERIA_MS 5000

int main() {
    stdio_init_all();
    sleep_ms(1000); // Pequena pausa para estabilizar

    // --- INICIALIZAÇÃO DE TODOS OS SISTEMAS ---
    printf("Sistema de captura de audio e monitor de bateria iniciado...\n");

    // Inicializa o microfone
    Microfone_t mic;
    if (Microfone_init(&mic) != MIC_OK) {
        printf("Erro ao inicializar o microfone.\n");
        while (1); // Trava o programa em caso de erro
    }
    printf("Microfone inicializado. Iniciando captura...\n");

    // Inicializa a bateria
    Bateria_inicializar();
    printf("Monitor de bateria inicializado.\n");
    printf("----------------------------------------\n");

    // Variável para controlar o tempo da próxima leitura da bateria
    // Inicializa para que a primeira leitura seja imediata.
    absolute_time_t proxima_leitura_bateria = get_absolute_time();

    while (1) {
        // --- Tarefas do Microfone (a cada 100ms) ---
        Microfone_capture_block(&mic);
        float rms = Microfone_rms_voltage(&mic);
        float peak = Microfone_peak_voltage(&mic);
        float freq = Microfone_frequency_estimate(&mic, SAMPLE_RATE);
        printf("Microfone -> RMS: %.3f V, Pico: %.3f V, Frequencia: %.1f Hz\n", rms, peak, freq);

        // --- Tarefas da Bateria (a cada 5 segundos) ---
        
        // CORREÇÃO: Usa time_reached() para verificar se o tempo agendado já passou.
        // Esta é a forma correta e mais segura de fazer a verificação.
        if (time_reached(proxima_leitura_bateria)) {
            
            // Lê os valores da bateria
            float tensao = Bateria_ler_tensao();
            float nivel = Bateria_ler_nivel();

            // Imprime os resultados da bateria
            printf("Bateria   -> Tensao: %.2fV, Nivel: %.1f%%\n", tensao, nivel);
            
            if (nivel < 20.0f) {
                printf("!!! ALERTA: BATERIA FRACA !!!\n");
            }
            printf("----------------------------------------\n");

            // Agenda a próxima leitura para daqui a 5 segundos a partir de AGORA
            proxima_leitura_bateria = make_timeout_time_ms(INTERVALO_BATERIA_MS);
        }

        // Delay principal do loop
        sleep_ms(100);
    }

    return 0;
}