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

float opt4001_read_lux_precise() {
    uint8_t msb, lsb, exponent;
    uint8_t reg;

    // Lê MSB
    reg = REG_RESULT_MSB;
    i2c_write_blocking(I2C_PORT, OPT4001_ADDR, &reg, 1, true);
    i2c_read_blocking(I2C_PORT, OPT4001_ADDR, &msb, 1, false);

    // Lê LSB
    reg = REG_RESULT_LSB;
    i2c_write_blocking(I2C_PORT, OPT4001_ADDR, &reg, 1, true);
    i2c_read_blocking(I2C_PORT, OPT4001_ADDR, &lsb, 1, false);

    // Lê EXPONENT
    reg = REG_EXPONENT;
    i2c_write_blocking(I2C_PORT, OPT4001_ADDR, &reg, 1, true);
    i2c_read_blocking(I2C_PORT, OPT4001_ADDR, &exponent, 1, false);

    // Monta valor MANTISSA e converte para lux
    uint32_t mantissa = ((uint16_t)msb << 8) | lsb;
    uint32_t adc_code = mantissa << exponent;

    // Multiplica pelo passo mínimo (assumindo versão SOT-5X3)
    return adc_code * 0.0004375f;
}

void opt4001_trigger_one_shot() {
    uint8_t config_data[3];
    config_data[0] = REG_CONFIGURATION;
    config_data[1] = 0xC0;  // OPERATING_MODE = 2 (one-shot sem redefinir faixa)
    config_data[2] = 0x10;  // Bits reservados padrão
    i2c_write_blocking(I2C_PORT, OPT4001_ADDR, config_data, 3, false);
}

uint8_t opt4001_read_counter() {
    uint8_t reg = REG_COUNTER;
    uint8_t counter;
    i2c_write_blocking(I2C_PORT, OPT4001_ADDR, &reg, 1, true);
    i2c_read_blocking(I2C_PORT, OPT4001_ADDR, &counter, 1, false);
    return counter & 0x0F; // só os 4 bits menos significativos
}

float opt4001_read_lux_one_shot() {
    uint8_t prev_counter = opt4001_read_counter();

    opt4001_trigger_one_shot();

    // Aguarda nova conversão (timeout simples de 100 tentativas)
    for (int i = 0; i < 100; i++) {
        sleep_ms(2); // ou um delay ajustável baseado no tempo de conversão
        uint8_t curr_counter = opt4001_read_counter();
        if (curr_counter != prev_counter) {
            return opt4001_read_lux_precise();  // já implementada na etapa anterior
        }
    }

    // Timeout — falha na medição
    return -1.0f;
}

void opt4001_set_thresholds(uint16_t result_high, uint8_t exp_high, uint16_t result_low, uint8_t exp_low) {
    uint8_t buf[3];

    // THRESHOLD_H_RESULT
    buf[0] = REG_THRESHOLD_H_RESULT;
    buf[1] = (result_high >> 8) & 0xFF;
    buf[2] = result_high & 0xFF;
    i2c_write_blocking(I2C_PORT, OPT4001_ADDR, buf, 3, false);

    // THRESHOLD_H_EXPONENT
    buf[0] = REG_THRESHOLD_H_EXPONENT;
    buf[1] = exp_high;
    i2c_write_blocking(I2C_PORT, OPT4001_ADDR, buf, 2, false);

    // THRESHOLD_L_RESULT
    buf[0] = REG_THRESHOLD_L_RESULT;
    buf[1] = (result_low >> 8) & 0xFF;
    buf[2] = result_low & 0xFF;
    i2c_write_blocking(I2C_PORT, OPT4001_ADDR, buf, 3, false);

    // THRESHOLD_L_EXPONENT
    buf[0] = REG_THRESHOLD_L_EXPONENT;
    buf[1] = exp_low;
    i2c_write_blocking(I2C_PORT, OPT4001_ADDR, buf, 2, false);
}

bool opt4001_is_above_threshold() {
    uint8_t reg = REG_FLAG_H;
    uint8_t val;
    i2c_write_blocking(I2C_PORT, OPT4001_ADDR, &reg, 1, true);
    i2c_read_blocking(I2C_PORT, OPT4001_ADDR, &val, 1, false);
    return val & 0x01;
}

bool opt4001_is_below_threshold() {
    uint8_t reg = REG_FLAG_L;
    uint8_t val;
    i2c_write_blocking(I2C_PORT, OPT4001_ADDR, &reg, 1, true);
    i2c_read_blocking(I2C_PORT, OPT4001_ADDR, &val, 1, false);
    return val & 0x01;
}

// Define faixa entre 500 lux e 50.000 lux
void configurar_limites_opt4001() {
    // Lux = MANTISSA << EXPONENT × passo
    // Para simplificar, defina MANTISSA=1 e ajuste apenas EXPONENT
    opt4001_set_thresholds(
        114, 10, // THRESHOLD_H: ~50.000 lux
        57, 7    // THRESHOLD_L: ~500 lux
    );
}

void opt4001_enable_threshold_interrupt() {
    uint8_t buf[2];

    // Configura INT como saída (INT_DIR = 1)
    buf[0] = REG_INTERRUPT_DIR;
    buf[1] = 0x01;
    i2c_write_blocking(I2C_PORT, OPT4001_ADDR, buf, 2, false);

    // Configura interrupção por thresholds (INT_CFG = 0)
    buf[0] = REG_INTERRUPT_CONFIG;
    buf[1] = 0x00;
    i2c_write_blocking(I2C_PORT, OPT4001_ADDR, buf, 2, false);
}

/// -----


// Calcula CRC-8 com polinômio 0x31 e valor inicial 0xFF (padrão SMBus)
uint8_t opt4001_crc8(const uint8_t *data, size_t len) {
    uint8_t crc = 0xFF;
    for (size_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (uint8_t b = 0; b < 8; b++) {
            if (crc & 0x80)
                crc = (crc << 1) ^ 0x31;
            else
                crc <<= 1;
        }
    }
    return crc;
}


void opt4001_enable_fifo_interrupt(uint8_t fifo_level) {
    uint8_t buf[2];

    // 1. Saída de interrupção como pulso (INT_PULSE = 1, INT_DIR = 1)
    buf[0] = REG_INTERRUPT_DIR;
    buf[1] = 0x03;  // Pulso, ativo em nível alto
    i2c_write_blocking(I2C_PORT, OPT4001_ADDR, buf, 2, false);

    // 2. Modo de interrupção com base no FIFO
    buf[0] = REG_INTERRUPT_CONFIG;
    buf[1] = 0x02;  // INT_CFG = 2 = FIFO
    i2c_write_blocking(I2C_PORT, OPT4001_ADDR, buf, 2, false);

    // 3. Configura nível desejado do FIFO (0~7)
    buf[0] = REG_FIFO_CONFIG;
    buf[1] = fifo_level & 0x07;
    i2c_write_blocking(I2C_PORT, OPT4001_ADDR, buf, 2, false);
}

// Lê quantas amostras estão no FIFO
uint8_t opt4001_fifo_count() {
    uint8_t reg = REG_FIFO_COUNT;
    uint8_t count;
    i2c_write_blocking(I2C_PORT, OPT4001_ADDR, &reg, 1, true);
    i2c_read_blocking(I2C_PORT, OPT4001_ADDR, &count, 1, false);
    return count & 0x07;  // São apenas 3 bits válidos
}

// Lê uma amostra do FIFO e calcula o lux, com verificação CRC
bool opt4001_read_fifo_sample(float *lux_out) {
    uint8_t reg = REG_RESULT;
    uint8_t buf[4];

    i2c_write_blocking(I2C_PORT, OPT4001_ADDR, &reg, 1, true);
    i2c_read_blocking(I2C_PORT, OPT4001_ADDR, buf, 4, false);

    uint8_t crc_calc = opt4001_crc8(buf, 3);
    if (crc_calc != buf[3]) {
        return false;  // CRC inválido
    }

    uint16_t mantissa = ((uint16_t)buf[0] << 8) | buf[1];
    uint8_t exponent = buf[2];
    *lux_out = mantissa * 0.01f * (1 << exponent);
    return true;
}

// Lê todo o FIFO em sequência
int opt4001_read_fifo_all(float *lux_array, int max_samples) {
    int count = opt4001_fifo_count();
    if (count > max_samples) count = max_samples;

    int valid = 0;
    for (int i = 0; i < count; i++) {
        if (opt4001_read_fifo_sample(&lux_array[valid])) {
            valid++;
        }
    }
    return valid;  // Retorna número de leituras válidas
}