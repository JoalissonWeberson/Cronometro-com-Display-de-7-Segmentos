#include <Arduino.h>

#define numDisplay 3          // Número de displays
#define numSegmentos 7        // Número de segmentos por display
#define numRepresentados 10   // Números representados (0 a 9)
#define tempoMultiplexador 30 // O tempo de altenacao entre os multiplexadores (30ms)

const uint8_t multiplexador[numDisplay] = {2, 3, 4}; // Pinos conectados aos displays
uint8_t estadoDisplays[numDisplay] = {0, 0, 0};
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

void exibirNumero(int n);
void cronometro();

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

    randomSeed(analogRead(0));
}

void loop()
{
    cronometro();
}

void exibirNumero(int n)
{
    for (int i = 0; i < numSegmentos; i++)
    {
        digitalWrite(leds[i], matrizSegmentos[n][i]);
    }
}

void cronometro()
{
    unsigned long tempoInicial = millis();
    estadoDisplays[0] += 1;
    if (estadoDisplays[0] > 9)
    {
        estadoDisplays[0] = 0;
        estadoDisplays[1] += 1;
    }
    if (estadoDisplays[1] > 9)
    {
        estadoDisplays[1] = 0;
        estadoDisplays[2] += 1;
    }
    if (estadoDisplays[2] > 9)
    {
        // Ver oq tem que ser feito
        estadoDisplays[0] = 9;
        estadoDisplays[1] = 9;
        estadoDisplays[2] = 9;
    }

    while (millis() - tempoInicial < 1000)
    {
        for (int j = 0; j < numDisplay; j++)
        {
            digitalWrite(multiplexador[j], HIGH);
            exibirNumero(estadoDisplays[j]);
            delay(tempoMultiplexador);
            digitalWrite(multiplexador[j], LOW);
        }
    }
}
