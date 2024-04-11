#include <Arduino.h>

#define numDisplay 3 // Número de displays
#define numSegmentos 7 // Número de segmentos por display
#define numRepresentados 10 // Números representados (0 a 9)

const uint8_t multiplexador[numDisplay] = {2, 3, 4}; // Pinos conectados aos displays
const uint8_t leds[numSegmentos] = {5, 6, 7, 8, 9, 10, 11}; // Pinos conectados aos segmentos
const uint8_t matrizSegmentos[numRepresentados][numSegmentos] = {
    {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, LOW},  // 0
    {LOW, HIGH, HIGH, LOW, LOW, LOW, LOW},      // 1
    {HIGH, HIGH, LOW, HIGH, HIGH, LOW, HIGH},   // 2
    {HIGH, HIGH, HIGH, HIGH, LOW, LOW, HIGH},   // 3
    {LOW, HIGH, HIGH, LOW, LOW, HIGH, HIGH},    // 4
    {HIGH, LOW, HIGH, HIGH, LOW, HIGH, HIGH},   // 5
    {HIGH, LOW, HIGH, HIGH, HIGH, HIGH, HIGH},  // 6
    {HIGH, HIGH, HIGH, LOW, LOW, LOW, LOW},     // 7
    {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH}, // 8
    {HIGH, HIGH, HIGH, HIGH, LOW, HIGH, HIGH}   // 9
};

void setup()
{
    // Configuração dos pinos dos displays como saídas
    for (int i = 0; i < numDisplay; i++)
    {
        pinMode(multiplexador[i], OUTPUT);
    }

    // Configuração dos pinos dos segmentos como saídas
    for (int i = 0; i < numSegmentos; i++)
    {
        pinMode(leds[i], OUTPUT);
    }
}

void loop()
{
    
}
