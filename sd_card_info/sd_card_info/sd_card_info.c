#include "sd_card_info.h"
#include "hardware/spi.h"

static spi_inst_t *spi = spi0;

// Inicialização básica do SPI
bool sd_init() {
    // Configurar GPIOs
    spi_init(spi, 1000 * 1000); // 1 MHz
    gpio_set_function(SD_MISO_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SD_MOSI_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SD_SCK_PIN, GPIO_FUNC_SPI);
    
    gpio_init(SD_CS_PIN);
    gpio_init(SD_DETECT_PIN);
    gpio_set_dir(SD_CS_PIN, GPIO_OUT);
    gpio_set_dir(SD_DETECT_PIN, GPIO_IN);
    gpio_pull_up(SD_DETECT_PIN);
    
    gpio_put(SD_CS_PIN, 1); // Desativa CS
    return true;
}

// Verifica presença física do cartão
bool sd_is_present() {
    return !gpio_get(SD_DETECT_PIN); // Ativo baixo (0 = presente)
}

// Função auxiliar para enviar comando
static void sd_command(uint8_t cmd, uint32_t arg, uint8_t crc) {
    uint8_t tx_buf[6] = {
        cmd,
        (arg >> 24) & 0xFF,
        (arg >> 16) & 0xFF,
        (arg >> 8) & 0xFF,
        arg & 0xFF,
        crc
    };
    
    gpio_put(SD_CS_PIN, 0);
    spi_write_blocking(spi, tx_buf, 6);
    gpio_put(SD_CS_PIN, 1);
}

// Obtém capacidade em bytes (versão simplificada)
uint64_t sd_get_capacity_bytes() {
    if (!sd_is_present()) return 0;
    
    // Comando básico para ler capacidade (CMD9)
    sd_command(0x49, 0, 0x01); // CMD9 (SEND_CSD)
    
    // Resposta simples (ignorando detalhes complexos)
    uint8_t response[16];
    gpio_put(SD_CS_PIN, 0);
    spi_read_blocking(spi, 0xFF, response, 16);
    gpio_put(SD_CS_PIN, 1);
    
    // Cálculo simplificado de capacidade
    uint32_t c_size = ((response[7] & 0x3F) << 16) | (response[8] << 8) | response[9];
    return (uint64_t)(c_size + 1) * 512 * 1024; // Para cartões SDSC
}

// Obtém capacidade em MB
float sd_get_capacity_mb() {
    return sd_get_capacity_bytes() / (1024.0f * 1024.0f);
}