#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

// Configurações da Rede
const char* nomeRede = "Alexandre";
const char* senhaRede = "xawc8090";

// URL do Servidor PHP
const String urlServidor = "http://172.19.243.39/projeto_sesi/salvar_dados.php";

#define PINO_SENSOR_HALL 4

volatile unsigned long tempoUltimoPulso = 0;
volatile unsigned long intervaloPulsos = 0;
volatile bool primeiroPulso = true;

float diametroRoda = 0.60;
float circunferenciaRoda;
float velocidadeKmh = 0;
float rpm = 0;

unsigned long tempoUltimoEnvio = 0;
// OTIMIZAÇÃO: Mudado de 2000 para 300 para termos atualização em tempo real
const unsigned long intervaloEnvio = 300; 

void IRAM_ATTR detectarIma() {
    unsigned long tempoAtual = micros();
    // Debounce de 5ms para evitar falsas leituras (ruído do sensor)
    if (tempoAtual - tempoUltimoPulso > 5000) {
        if (!primeiroPulso) {
            intervaloPulsos = tempoAtual - tempoUltimoPulso;
        } else {
            primeiroPulso = false;
        }
        tempoUltimoPulso = tempoAtual;
    }
}

void setup() {
    Serial.begin(115200);
    
    WiFi.begin(nomeRede, senhaRede);
    Serial.print("Conectando ao Wi-Fi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConectado com sucesso!");
    
    pinMode(PINO_SENSOR_HALL, INPUT_PULLUP);
    circunferenciaRoda = diametroRoda * 3.141592;
    attachInterrupt(digitalPinToInterrupt(PINO_SENSOR_HALL), detectarIma, FALLING);
}

void loop() {
    // Proteção contra concorrência de memória (Interrupções)
    noInterrupts();
    unsigned long intervalo = intervaloPulsos;
    unsigned long tempoDesdeUltimoPulso = micros() - tempoUltimoPulso;
    interrupts();

    // Se a roda parar por mais de 2 segundos, zera tudo
    if (tempoDesdeUltimoPulso > 2000000 || primeiroPulso) {
        velocidadeKmh = 0;
        rpm = 0;
        noInterrupts();
        intervaloPulsos = 0;
        primeiroPulso = true;
        interrupts();
    }
    else if (intervalo > 0) {
        float pulsosPorSegundo = 1000000.0 / intervalo;
        rpm = pulsosPorSegundo * 60.0;
        // Velocidade = Circunferência * RPS * 3.6 (para converter m/s para km/h)
        velocidadeKmh = (circunferenciaRoda * pulsosPorSegundo) * 3.6;
    }

    // Envio dos dados via HTTP POST
    if (millis() - tempoUltimoEnvio > intervaloEnvio) {
        tempoUltimoEnvio = millis();
        
        if (WiFi.status() == WL_CONNECTED) {
            HTTPClient clienteHttp;
            
            clienteHttp.begin(urlServidor);
            clienteHttp.addHeader("Content-Type", "application/x-www-form-urlencoded");
            
            // Monta os dados com 2 casas decimais
            String dadosRequisicao = "velocidade=" + String(velocidadeKmh, 2) + "&rpm=" + String(rpm, 2);
            
            int codigoRespostaHttp = clienteHttp.POST(dadosRequisicao);
            
            if (codigoRespostaHttp > 0) {
                String respostaServidor = clienteHttp.getString();
                Serial.println("Resposta do Servidor: " + respostaServidor);
            } else {
                Serial.printf("Erro ao enviar POST: %s\n", clienteHttp.errorToString(codigoRespostaHttp).c_str());
            }
            
            // CORREÇÃO CRÍTICA: Fecha a conexão HTTP para liberar a memória do ESP32!
            clienteHttp.end(); 
        } else {
            Serial.println("Wi-Fi desconectado! Tentando reconectar...");
            WiFi.begin(nomeRede, senhaRede);
        }
    }
}