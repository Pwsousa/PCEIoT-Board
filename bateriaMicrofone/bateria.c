#include "bateria.h"
#include "hardware/adc.h"

#define PINO_BATERIA 28      // GP28 (ADC2)
#define DIVISOR_TENSAO 2.0f  // R25 e R28 (4.7MΩ cada), divide a tensão por 2 [cite: 718]
#define REFERENCIA_ADC 2.5f  // CORRIGIDO: U6 fornece uma referência de 2.5V 
#define RESOLUCAO_ADC 4095.0f // Resolução de 12 bits (0-4095)

// Parâmetros da bateria Li-Po
#define VOLTAGEM_MAX 4.2f    // Tensão máxima da bateria
#define VOLTAGEM_MIN 3.0f    // Tensão mínima segura

void Bateria_inicializar(void) {
    adc_init();
    adc_gpio_init(PINO_BATERIA);
    adc_select_input(2);     // Seleciona ADC2 (GP28)
}

float Bateria_ler_tensao(void) {
    // Lê o valor bruto do ADC (0-4095)
    uint16_t leitura = adc_read();
    
    // Converte para tensão no pino do ADC
    float tensao_adc = (leitura * REFERENCIA_ADC) / RESOLUCAO_ADC;
    
    // Calcula a tensão real da bateria considerando o divisor resistivo
    return tensao_adc * DIVISOR_TENSAO;
}

float Bateria_ler_nivel(void) {
    float tensao = Bateria_ler_tensao();
    
    // Limita a tensão aos valores máximos/minimos
    if (tensao >= VOLTAGEM_MAX) return 100.0f;
    if (tensao <= VOLTAGEM_MIN) return 0.0f;
    
    // Calcula a porcentagem baseada na curva de descarga
    return (tensao - VOLTAGEM_MIN) / (VOLTAGEM_MAX - VOLTAGEM_MIN) * 100.0f;
}