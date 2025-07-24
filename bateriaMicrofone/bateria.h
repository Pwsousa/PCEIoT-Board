#ifndef BATERIA_H
#define BATERIA_H

#include "pico/stdlib.h"

// Inicializa o sistema de monitoramento da bateria
void Bateria_inicializar(void);

// Lê a tensão atual da bateria em Volts
float Bateria_ler_tensao(void);

// Lê o nível da bateria em porcentagem (0-100%)
float Bateria_ler_nivel(void);

#endif
