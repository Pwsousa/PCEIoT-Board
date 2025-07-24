#ifndef MICROFONE_H
#define MICROFONE_H

#include "hardware/adc.h"
#include <stdint.h>

// --- Configurações ---
#define MIC_PIN         28      // Pino do ADC (GPIO28 é ADC2)
#define SAMPLE_RATE     10000   // Taxa de amostragem em Hz
#define BUFFER_SIZE     256     // Tamanho do buffer de amostragem

// --- Tipos e Enums ---
typedef enum {
    MIC_OK,
    MIC_ADC_ERROR
} mic_status_t;

typedef struct {
    uint16_t raw_buffer[BUFFER_SIZE];
    float voltage_buffer[BUFFER_SIZE];
    float dc_offset;
    uint32_t sample_count;
} Microfone_t;

// --- Protótipos das Funções Públicas ---

/**
 * @brief Inicializa o hardware do ADC para o microfone.
 * @param mic Ponteiro para a estrutura do microfone.
 * @return MIC_OK se bem-sucedido.
 */
mic_status_t Microfone_init(Microfone_t *mic);

/**
 * @brief Captura um bloco de amostras do ADC de forma bloqueante.
 * @param mic Ponteiro para a estrutura do microfone.
 */
void Microfone_capture_block(Microfone_t *mic);

/**
 * @brief Lê uma única amostra de tensão do microfone.
 * @param mic Ponteiro para a estrutura do microfone.
 * @return A tensão lida em Volts.
 */
float Microfone_read_voltage(Microfone_t *mic);

/**
 * @brief Calcula a tensão RMS do último bloco de amostras capturado.
 * @param mic Ponteiro para a estrutura do microfone.
 * @return O valor da tensão RMS em Volts.
 */
float Microfone_rms_voltage(Microfone_t *mic);

/**
 * @brief Encontra a tensão de pico do último bloco de amostras capturado.
 * @param mic Ponteiro para a estrutura do microfone.
 * @return O valor da tensão de pico em Volts (absoluto).
 */
float Microfone_peak_voltage(Microfone_t *mic);

/**
 * @brief Estima a frequência fundamental do sinal usando contagem de cruzamentos por zero.
 * @param mic Ponteiro para a estrutura do microfone.
 * @param sample_rate A taxa de amostragem usada na captura.
 * @return A frequência estimada em Hertz.
 */
float Microfone_frequency_estimate(Microfone_t *mic, float sample_rate);

/**
 * @brief Remove o offset DC do buffer de tensão.
 * @param mic Ponteiro para a estrutura do microfone.
 */
void Microfone_remove_dc_offset(Microfone_t *mic);

#endif // MICROFONE_H