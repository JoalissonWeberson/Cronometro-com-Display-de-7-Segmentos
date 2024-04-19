/* Este código é um cronômetro com botões de start/pausa e reset,
   utilizando displays de 7 segmentos e um buzzer */

#include <Arduino.h>

#define numDisplay 3          // Número de displays
#define numSegmentos 7        // Número de segmentos por display
#define numRepresentados 10   // Números representados (0 a 9)
#define numBtn 2              // Número de botões
#define tempoMultiplexador 30 // O tempo de alternância entre os multiplexadores (30ms)

// Estados da máquina de estado
#define ESTADO_INIT 0    // Estado inicial
#define ESTADO_PARADO 1  // Estado parado
#define ESTADO_RODANDO 2 // Estado rodando
#define ESTADO_BUZZER 3  // Estado buzzer

// Facilitar o Controle dos botões
#define btn_start 0 // start/pausa
#define btn_reset 1 // reset

#define BUZZER_PIN -1 // Pino do Buzzer de alerta (não utilizado)

uint8_t estadoAtual;                                        // Estado atual da máquina de estado
uint8_t estadoDisplays[numDisplay] = {0, 0, 0};             // Número atual em cada display
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

unsigned long tempoDebounce = 50;                     // Tempo de Debounce
unsigned long tempoDebounceAnterior[numBtn] = {0, 0}; // Tempo anterior de debounce de cada botão

const uint8_t btns[numBtn] = {12, 13};

uint8_t leituraAnterior[numBtn];        // A leitura anterior do pino de entrada
uint8_t estadoBtn[numBtn] = {LOW, LOW}; // A leitura atual do pino de entrada

// Funções Protótipo
void exibirNumero();               // Função responsável por exibir o número que está no display
void cronometro();                 // Função responsável por realizar a contagem do cronômetro
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

    // Configuração dos pinos dos botões
    for (int i = 0; i < numBtn; i++)
    {
        pinMode(btns[i], INPUT);
    }

    // Configura todas as leituras anteriores como baixas
    for (int i = 0; i < numBtn; i++)
    {
        leituraAnterior[i] = LOW;
    }

    // pinMode(BUZZER_PIN, OUTPUT); // Pino do Buzzer (não utilizado)

    estadoAtual = ESTADO_INIT;
}

void loop()
{
    // Verifica qual o estado da máquina em cada loop
    switch (estadoAtual)
    {
    case ESTADO_INIT:
        // Zera os valores do display
        for (int i = 0; i < numDisplay; i++)
        {
            estadoDisplays[i] = 0;
        }

        // Muda o estado para parado
        estadoAtual = ESTADO_PARADO;
        break;
    case ESTADO_PARADO:

        // Responsável por exibir o número
        exibirNumero();

        // Verifica se o botão de start foi pressionado
        if (verificarBtn(btn_start) == true)
        {
            // Muda de estado para rodando
            estadoAtual = ESTADO_RODANDO;
        }
        // Caso o botão start não esteja pressionado, verificar se o botão de reset está pressionado
        else if (verificarBtn(btn_reset) == true)
        {
            // Muda de estado para inicial
            estadoAtual = ESTADO_INIT;
        }
        break;
    case ESTADO_RODANDO:
        // Começa a fazer a incrementação nos display
        cronometro();

        // Verifica se o botão de start foi pressionado
        if (verificarBtn(btn_start) == true)
        {
            // Muda o estado para parado
            estadoAtual = ESTADO_PARADO;

            // Previnir alteração rápida de estado
            unsigned long espera = millis();
            while (millis() - espera < 900)
            {
                cronometro();
            }
        }
        break;
    case ESTADO_BUZZER:
        // Emite um alarme
        emitirAlarme();
        exibirNumero();
        break;
    }
}

// Função para exibir o número atual nos displays de 7 segmentos
void exibirNumero()
{
    // Responsável por repetir para todos os displays
    for (int i = 0; i < numDisplay; i++)
    {
        digitalWrite(multiplexador[i], HIGH); // Liga o display correspondente

        // Liga os segmentos do display, exibindo o número desejado (controlado por estadoDisplays)
        for (int j = 0; j < numSegmentos; j++)
        {
            digitalWrite(leds[j], matrizSegmentos[estadoDisplays[i]][j]);
        }

        delay(tempoMultiplexador);           // Espera um curto tempo para mudar de display
        digitalWrite(multiplexador[i], LOW); // Desliga o display
    }
}

// Função para realizar a contagem do cronômetro
void cronometro()
{
    unsigned long tempoInicial = millis(); // Captura o tempo que começou

    // Verifica se o cronômetro chegou ao limite
    if (estadoDisplays[2] > 9)
    {
        estadoDisplays[0] = 9;
        estadoDisplays[1] = 9;
        estadoDisplays[2] = 9;
        estadoAtual = ESTADO_BUZZER; // Mudar o estado para Buzzer
    }
    else if (estadoDisplays[1] > 9) // Verifica se o segundo display atingiu o valor limite (9), atualizando o terceiro
    {
        estadoDisplays[1] = 0;
        estadoDisplays[2] += 1;
    }
    else if (estadoDisplays[0] > 9) // Verifica se o primeiro display atingiu o valor limite (9), atualizando o segundo
    {
        estadoDisplays[0] = 0;
        estadoDisplays[1] += 1;
    }
    
    // Exibe o número atual até passar um segundo
    while (millis() - tempoInicial < 1000)
    {
        exibirNumero();

        // Verifica se enquanto está rodando, o botão é pressionado
        if (verificarBtn(btn_start) == true)
        {
            if (estadoAtual == ESTADO_RODANDO)
            {
                estadoAtual = ESTADO_PARADO;
            }
        }
    }

    estadoDisplays[0] += 1; // Incrementa um segundo ao cronômetro
}

// Função para verificar se os botões foram pressionados e lidar com o debounce
bool verificarBtn(uint8_t button)
{
    // Ler o estado do botão para uma variável local:
    uint8_t leitura = digitalRead(btns[button]);

    // Verificar se houve uma mudança no estado do botão
    if (leitura != leituraAnterior[button])
    {
        // Redefinir o temporizador de debounce
        tempoDebounceAnterior[button] = millis();
    }

    if (millis() - tempoDebounceAnterior[button] > tempoDebounce)
    {
        // Se passou tempo suficiente desde a última verificação, consideramos como uma mudança válida
        if (leitura != estadoBtn[button])
        {
            estadoBtn[button] = leitura;
            if (leitura == HIGH)
            {
                leituraAnterior[button] = leitura;
                return true; // Retorna true se o novo estado do botão for ALTO
            }
        }
    }

    leituraAnterior[button] = leitura;
    return false;
}

// Função para emitir um alarme usando o buzzer (não utilizado no momento)
void emitirAlarme()
{
    // Emite o som do alarme (pode variar de acordo com o buzzer)
    tone(BUZZER_PIN, 1000); // Emite um som de 1000 Hz
    delay(500);             // Mantém o som por 500 milissegundos
    noTone(BUZZER_PIN);     // Desliga o som
    delay(500);             // Aguarda 500 milissegundos antes de repetir o som
}
