#include "Microfone.h"
#include <math.h>
#include <stdbool.h>

// Função corrigida: agora contém a lógica de inicialização.
mic_status_t Microfone_init(Microfone_t *mic) {
    adc_init(); // Inicializa o periférico ADC

    // Inicializa o pino GPIO para a função de ADC
    adc_gpio_init(MIC_PIN);

    // Seleciona o canal do ADC (ADC2 corresponde ao pino GPIO28)
    adc_select_input(2);

    // Inicializa os campos da struct
    mic->dc_offset = 0.0f;
    mic->sample_count = 0;

    return MIC_OK;
}

void Microfone_capture_block(Microfone_t *mic) {
    for (int i = 0; i < BUFFER_SIZE; i++) {
        mic->raw_buffer[i] = adc_read();
        // Converte o valor de 12-bit para tensão (3.3V é a referência)
        mic->voltage_buffer[i] = mic->raw_buffer[i] * 3.3f / 4095.0f;
    }
    // Remove o componente DC antes de qualquer cálculo
    Microfone_remove_dc_offset(mic);
    mic->sample_count += BUFFER_SIZE;
}

float Microfone_read_voltage(Microfone_t *mic) {
    uint16_t raw = adc_read();
    return raw * 3.3f / 4095.0f;
}

void Microfone_remove_dc_offset(Microfone_t *mic) {
    // Calcula a média (offset DC) do sinal no buffer
    float sum = 0.0f;
    for (int i = 0; i < BUFFER_SIZE; i++) {
        sum += mic->voltage_buffer[i];
    }
    mic->dc_offset = sum / BUFFER_SIZE;

    // Remove o offset DC de cada amostra no buffer
    for (int i = 0; i < BUFFER_SIZE; i++) {
        mic->voltage_buffer[i] -= mic->dc_offset;
    }
}

float Microfone_rms_voltage(Microfone_t *mic) {
    float sum_squares = 0.0f;
    for (int i = 0; i < BUFFER_SIZE; i++) {
        sum_squares += mic->voltage_buffer[i] * mic->voltage_buffer[i];
    }
    return sqrtf(sum_squares / BUFFER_SIZE);
}

float Microfone_peak_voltage(Microfone_t *mic) {
    float peak = 0.0f;
    for (int i = 0; i < BUFFER_SIZE; i++) {
        if (fabsf(mic->voltage_buffer[i]) > peak) {
            peak = fabsf(mic->voltage_buffer[i]);
        }
    }
    return peak;
}

float Microfone_frequency_estimate(Microfone_t *mic, float sample_rate) {
    // Implementação simples de contagem de cruzamentos por zero
    int crossings = 0;
    for (int i = 1; i < BUFFER_SIZE; i++) {
        // Verifica se houve uma mudança de sinal
        if ((mic->voltage_buffer[i - 1] < 0 && mic->voltage_buffer[i] >= 0) ||
            (mic->voltage_buffer[i - 1] > 0 && mic->voltage_buffer[i] <= 0)) {
            crossings++;
        }
    }
    // A frequência é metade do número de cruzamentos por segundo
    return (crossings * sample_rate) / (2.0f * BUFFER_SIZE);
}