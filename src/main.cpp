#include <Arduino.h>

#define numDisplay 3          // Número de displays
#define numSegmentos 7        // Número de segmentos por display
#define numRepresentados 10   // Números representados (0 a 9)
#define numBtn 3              // Número de botões
#define tempoMultiplexador 30 // O tempo de alternação entre os multiplexadores (30ms)

// Estados da máquina de estado
#define ESTADO_INIT 0    // Estado inicial
#define ESTADO_PARADO 1  // Estado parado
#define ESTADO_RODANDO 2 // Estado rodando
#define ESTADO_BUZZER 3  // Estado buzzer

// Facilitar o Controle dos botões
#define btn_pausa 0
#define btn_start 1
#define btn_reset 2

//(não tem pino disponivel)
#define BUZZER_PIN 0 // Pino do Buzzer de alerta (não tem pino disponivel)

uint8_t estadoAtual;                                        // Estado atual da maquina de estado
uint8_t estadoDisplays[numDisplay] = {0, 0, 0};             // Numero atual em cada display
const uint8_t multiplexador[numDisplay] = {2, 3, 4};        // Pinos conectados aos displays
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

unsigned long tempoDebounce = 50;                        // Tempo de Debounce
unsigned long tempoDebounceAnterior[numBtn] = {0, 0, 0}; // Tempo anterior de debounce de cada botão

//(não tem pino disponivel para o 1)
const uint8_t btns[numBtn] = {1, 12, 13};

uint8_t leituraAnterior[numBtn];             // a leitura anterior do pino de entrada
uint8_t estadoBtn[numBtn] = {LOW, LOW, LOW}; // // a leitura atual do pino de entrada

void exibirNumero();               // Função responsavel por exibe o numero que esta no display
void cronometro();                 // Função responsavel por realizar a contagem do cronometro
void emitirAlarme();               // Emite um alarme com Buzzer
bool verificarBtn(uint8_t button); // Prevenção de Debounce

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

    // Configuração dos pinos dos botoes
    for (int i = 0; i < numBtn; i++)
    {
        pinMode(btns[i], INPUT);
    }

    // Configura todas leituras anteriores como baixo
    for (int i = 0; i < numBtn; i++)
    {
        leituraAnterior[i] = LOW;
    }

    pinMode(BUZZER_PIN, OUTPUT);

    estadoAtual = ESTADO_INIT;
}

void loop()
{

    switch (estadoAtual)
    {
    case ESTADO_INIT:
        for (int i = 0; i < numDisplay; i++)
        {
            estadoDisplays[i] = 0;
        }

        estadoAtual = ESTADO_PARADO;
        break;
    case ESTADO_PARADO:
        while (true)
        {
            exibirNumero();
            if (verificarBtn(btn_start) == true)
            {
                if (estadoAtual == ESTADO_PARADO)
                {
                    estadoAtual = ESTADO_RODANDO;
                    break;
                }
            }
            if (verificarBtn(btn_reset) == true)
            {
                estadoAtual = ESTADO_INIT;
                break;
            }
        }
        break;
    case ESTADO_RODANDO:
        cronometro();

        if (verificarBtn(btn_start) == true)
        {
            if (estadoAtual == ESTADO_RODANDO)
            {
                estadoAtual = ESTADO_PARADO;
                // Previnir alteração rapida de estado
                unsigned long espera = millis();
                while(millis() - espera < 1000){
                    cronometro();
                }
            }
        }
        break;
    case ESTADO_BUZZER:
        void emitirAlarme();
        break;
    }
}

void exibirNumero()
{
    for (int i = 0; i < numDisplay; i++)
    {
        digitalWrite(multiplexador[i], HIGH);
        for (int j = 0; j < numSegmentos; j++)
        {
            digitalWrite(leds[j], matrizSegmentos[estadoDisplays[i]][j]);
        }
        delay(tempoMultiplexador);
        digitalWrite(multiplexador[i], LOW);
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
        estadoAtual = ESTADO_BUZZER;
    }

    while (millis() - tempoInicial < 1000)
    {
        exibirNumero();
    }
}

bool verificarBtn(uint8_t button)
{
    uint8_t leitura = digitalRead(btns[button]);

    if (leitura != leituraAnterior[button])
    {
        tempoDebounceAnterior[button] = millis();
    }

    if (millis() - tempoDebounceAnterior[button])
    {
        if (leitura != estadoBtn[button])
        {
            estadoBtn[button] = leitura;
            if (leitura == HIGH)
            {
                leituraAnterior[button] = leitura;
                return true;
            }
        }
    }

    leituraAnterior[button] = leitura;
    return false;
}

void emitirAlarme()
{
    // Emite o som do alarme (pode variar de acordo com o buzzer)
    tone(BUZZER_PIN, 1000); // Emite um som de 1000 Hz
    delay(500);             // Mantém o som por 500 milissegundos
    noTone(BUZZER_PIN);     // Desliga o som
    delay(500);             // Aguarda 500 milissegundos antes de repetir o som
}
