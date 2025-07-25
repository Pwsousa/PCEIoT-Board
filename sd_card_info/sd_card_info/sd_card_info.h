#ifndef SD_CARD_INFO_H
#define SD_CARD_INFO_H

#include "pico/stdlib.h"

// Pinos SPI conforme seu esquemático
#define SD_MISO_PIN 16
#define SD_MOSI_PIN 19
#define SD_SCK_PIN  18
#define SD_CS_PIN   17
#define SD_DETECT_PIN 15  // Pino de detecção física

// Funções simplificadas
bool sd_init();
bool sd_is_present();
uint64_t sd_get_capacity_bytes();
float sd_get_capacity_mb();

#endif