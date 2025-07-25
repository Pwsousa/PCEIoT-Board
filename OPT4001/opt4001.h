#ifndef OPT4001_H
#define OPT4001_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Estrutura que representa o sensor OPT4001.
 * 
 * Contém o descritor do I2C (adaptar conforme ambiente),
 * endereço do dispositivo e offset para calibração.
 */
typedef struct {
    i2c_inst_t *i2c;          /**< Descritor da interface I2C (se Linux) ou adaptar para Pico SDK */
    uint8_t address;     /**< Endereço I2C do sensor */
    float offset;        /**< Offset para calibração do valor de lux */
} OPT4001;

/**
 * @brief Inicializa o sensor OPT4001 na interface I2C informada.
 * 
 * Para Linux, abre o dispositivo I2C; para Pico SDK, adaptar inicialização.
 * 
 * @param sensor Ponteiro para a estrutura OPT4001 a inicializar.
 * @param i2c_path Caminho do dispositivo I2C (ex: "/dev/i2c-1") para Linux.
 *                 No Pico SDK, adaptar para interface i2c_inst_t*.
 * @param address Endereço I2C do sensor.
 * @return int 0 em sucesso, negativo em erro.
 */
int opt4001_init(OPT4001 *sensor, i2c_inst_t *i2c , uint8_t address);


/**
 * @brief Escreve um valor de 16 bits em um registrador do sensor.
 * 
 * @param sensor Ponteiro para o sensor.
 * @param reg Endereço do registrador.
 * @param value Valor a ser escrito.
 * @return int 0 em sucesso, negativo em falha.
 */
int opt4001_write_register(OPT4001 *sensor, uint8_t reg, uint16_t value);

/**
 * @brief Lê um valor de 16 bits de um registrador do sensor.
 * 
 * @param sensor Ponteiro para o sensor.
 * @param reg Endereço do registrador.
 * @param value Ponteiro para armazenar o valor lido.
 * @return int 0 em sucesso, negativo em falha.
 */
int opt4001_read_register(OPT4001 *sensor, uint8_t reg, uint16_t *value);

/**
 * @brief Lê os dados brutos do sensor, retornando valor e expoente.
 * 
 * @param sensor Ponteiro para o sensor.
 * @param raw Ponteiro para armazenar o valor bruto.
 * @param exp Ponteiro para armazenar o expoente.
 * @return int 0 em sucesso, negativo em falha.
 */
int opt4001_read_raw(OPT4001 *sensor, uint32_t *raw, uint8_t *exp);

/**
 * @brief Lê a iluminância atual em lux do sensor.
 * 
 * Calcula o lux aplicando offset e o cálculo do valor bruto e expoente.
 * 
 * @param sensor Ponteiro para o sensor.
 * @return float Valor de lux; negativo em erro.
 */
float opt4001_read_lux(OPT4001 *sensor);

/**
 * @brief Verifica se o sensor está conectado e respondendo.
 * 
 * @param sensor Ponteiro para o sensor.
 * @return bool true se conectado, false caso contrário.
 */
bool opt4001_is_connected(OPT4001 *sensor);

/**
 * @brief Obtém o ID do dispositivo do sensor.
 * 
 * @param sensor Ponteiro para o sensor.
 * @param id Ponteiro para armazenar o ID.
 * @return int 0 em sucesso, negativo em falha.
 */
int opt4001_get_device_id(OPT4001 *sensor, uint16_t *id);

/**
 * @brief Obtém o status atual do sensor.
 * 
 * @param sensor Ponteiro para o sensor.
 * @param status Ponteiro para armazenar o status.
 * @return int 0 em sucesso, negativo em falha.
 */
int opt4001_get_status(OPT4001 *sensor, uint16_t *status);

/**
 * @brief Envia comando para resetar o sensor.
 * 
 * @param sensor Ponteiro para o sensor.
 * @return int 0 em sucesso, negativo em falha.
 */
int opt4001_reset(OPT4001 *sensor);

/**
 * @brief Ajusta o offset para calibração nas leituras de lux.
 * 
 * @param sensor Ponteiro para o sensor.
 * @param offset Valor do offset em lux.
 */
void opt4001_set_offset(OPT4001 *sensor, float offset);

#endif 
