# PCEIoT-Board

# OPT4001 Raspberry Pi Pico Driver

Driver em C para o sensor de luz **Texas Instruments OPT4001**, utilizando a biblioteca padrão do Raspberry Pi Pico SDK.

Este driver fornece funções para inicialização, leitura de lux (aproximada e precisa), configuração de thresholds, leitura via FIFO e suporte a interrupções.

---

## 📦 Pré-requisitos

Antes de usar, você precisa ter:

- **Raspberry Pi Pico SDK** instalado e configurado  
- Compilador `arm-none-eabi-gcc` instalado  
- Ligação correta do sensor OPT4001 ao Pico:
  - **SDA** → GPIO 8 (padrão)
  - **SCL** → GPIO 9 (padrão)
  - **VCC** → 3V3
  - **GND** → GND

---

## 📥 Instalação

1. Clone este repositório:

```bash
git clone https://github.com/seuusuario/opt4001-pico.git
