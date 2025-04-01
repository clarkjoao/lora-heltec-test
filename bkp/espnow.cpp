// main.cpp – Mestre/Escravo ESP-NOW para ESP32
// Descomente a linha abaixo para compilar o código como MESTRE:
// #define AS_MASTER

#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>

// Definições gerais
const uint16_t MAGIC_WORD = 0xCAFE;      // MagicByte de 2 bytes para identificar pacotes
const uint8_t OPCODE_HELLO      = 1;     // OpCode 1: Handshake inicial ("hello" do escravo)
const uint8_t OPCODE_CONFIRMED  = 2;     // OpCode 2: Confirmação de registro (resposta do mestre)
const uint8_t OPCODE_ACTIVATE   = 3;     // OpCode 3: Ativar pino 2 dos escravos
const uint8_t OPCODE_DEACTIVATE = 4;     // OpCode 4: Desativar pino 2 dos escravos

// Estrutura do pacote ESP-NOW (3 bytes: 2 bytes magic + 1 byte opcode)
#pragma pack(push, 1)  // garantir que não haja padding
struct Message {
    uint16_t magic;
    uint8_t  opcode;
};
#pragma pack(pop)

// Variáveis globais dependentes do papel (mestre ou escravo)
#ifndef AS_MASTER
// Configurações para ESCRAVO
uint8_t broadcastAddress[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};  // endereço MAC de broadcast
uint8_t masterAddress[6];       // Armazena o MAC do mestre após registro
bool masterRegistered = false;  // Indica se mestre já foi registrado (handshake concluído)
#endif

#ifdef AS_MASTER
// Configurações para MESTRE
const int   MAX_SLAVES = 20;
uint8_t slaveAddresses[MAX_SLAVES][6];  // Lista de MACs de escravos conhecidos
int     slaveCount = 0;                 // Quantidade de escravos registrados
bool    sentActivation = false;         // Se comando de ativação (opcode 3) já foi enviado
bool    sentDeactivation = false;       // Se comando de desativação (opcode 4) já foi enviado
unsigned long firstPeerTime = 0;        // Momento em que o primeiro escravo foi registrado
unsigned long activationTime = 0;       // Momento em que enviamos o comando de ativação
#endif

// Função auxiliar para imprimir um MAC address em formato hexadecimal
void printMac(const uint8_t *mac) {
    char macStr[18];
    snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    Serial.print(macStr);
}

// Callback de envio (onSend): chamada quando um pacote é enviado (sucesso ou falha)
void onSend(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.print("onSend: MAC de destino ");
    if (mac_addr == NULL) {
      Serial.print("NULL");
    } else {
      printMac(mac_addr);
    }
    Serial.print(", Status: ");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Sucesso" : "Falha");
}

// Callback de recebimento (onReceive): chamada quando um pacote ESP-NOW é recebido
void onReceive(const uint8_t *mac, const uint8_t *incomingData, int len) {
    // Verifica tamanho mínimo do pacote
    if (len < sizeof(Message)) {
        Serial.println("Pacote recebido com tamanho inesperado, ignorando");
        return;
    }
    // Copia os dados recebidos para a estrutura Message
    Message msg;
    memcpy(&msg, incomingData, sizeof(msg));
    // Verifica o magicByte de segurança
    if (msg.magic != MAGIC_WORD) {
        Serial.println("MagicByte incorreto, pacote ignorado");
        return;
    }
    // Log do pacote recebido (MAC e opcode)
    Serial.print("Pacote recebido de ");
    printMac(mac);
    Serial.print(" com opcode ");
    Serial.println(msg.opcode);
#ifdef AS_MASTER
    // Lógica de processamento no MESTRE
    if (msg.opcode == OPCODE_HELLO) {
        Serial.println("Recebido pacote de handshake (hello) de um escravo");
        // Adiciona o escravo como peer caso ainda não esteja na lista
        if (!esp_now_is_peer_exist(mac)) {
            esp_now_peer_info_t peerInfo = {};
            memcpy(peerInfo.peer_addr, mac, 6);
            peerInfo.channel = 0;      // 0 = canal atual (assume mesmo canal do mestre)
            peerInfo.encrypt = false;
            if (esp_now_add_peer(&peerInfo) == ESP_OK) {
                Serial.println("Novo escravo adicionado à lista de peers");
                // Armazena MAC do escravo na lista local
                if (slaveCount < MAX_SLAVES) {
                    memcpy(slaveAddresses[slaveCount], mac, 6);
                    slaveCount++;
                }
                // Registra o tempo em que o primeiro escravo foi adicionado
                if (slaveCount == 1) {
                    firstPeerTime = millis();
                }
            } else {
                Serial.println("Falha ao adicionar escravo como peer");
            }
        } else {
            Serial.println("Escravo já estava na lista de peers");
        }
        // Envia pacote de confirmação de registro (opcode 2) de volta para o escravo
        Message outMsg;
        outMsg.magic  = MAGIC_WORD;
        outMsg.opcode = OPCODE_CONFIRMED;
        esp_err_t result = esp_now_send(mac, (uint8_t*)&outMsg, sizeof(outMsg));
        if (result == ESP_OK) {
            Serial.println("Pacote de confirmação enviado ao escravo");
        } else {
            Serial.println("Falha ao enviar pacote de confirmação ao escravo");
        }
    } else {
        Serial.println("Opcode inesperado recebido pelo mestre, ignorando");
    }
#else
    // Lógica de processamento no ESCRAVO
    if (msg.opcode == OPCODE_CONFIRMED && !masterRegistered) {
        // Recebeu confirmação de registro (handshake concluído)
        memcpy(masterAddress, mac, 6);   // salva MAC do mestre
        masterRegistered = true;
        Serial.print("Mestre ");
        printMac(mac);
        Serial.println(" registrado com sucesso.");
        // Remove peer de broadcast (não precisa mais ouvir broadcasts após handshake)
        esp_now_del_peer(broadcastAddress);
        // Adiciona o mestre como peer para comunicações diretas futuras
        esp_now_peer_info_t peerInfo = {};
        memcpy(peerInfo.peer_addr, mac, 6);
        peerInfo.channel = 0;      // mesmo canal atual
        peerInfo.encrypt = false;
        if (esp_now_add_peer(&peerInfo) == ESP_OK) {
            Serial.println("Mestre adicionado como peer para comunicação direta");
        } else {
            Serial.println("Falha ao adicionar mestre como peer");
        }
    } 
    else if (masterRegistered && memcmp(mac, masterAddress, 6) == 0) {
        // Pacote vindo do mestre já registrado
        if (msg.opcode == OPCODE_ACTIVATE) {
            Serial.println("Comando ATIVAR (opcode 3) recebido do mestre. Acionando pino 2.");
            digitalWrite(16, HIGH);  // ativa pino 2
        } else if (msg.opcode == OPCODE_DEACTIVATE) {
            Serial.println("Comando DESATIVAR (opcode 4) recebido do mestre. Desligando pino 2.");
            digitalWrite(16, LOW);   // desativa pino 2
        } else {
            Serial.println("Opcode desconhecido recebido do mestre, ignorando");
        }
    } 
    else {
        // Pacote não esperado (ex: de outro dispositivo) 
        Serial.println("Pacote de dispositivo desconhecido ou fora do contexto, ignorando");
    }
#endif
}

void setup() {
    Serial.begin(115200);
#ifdef AS_MASTER
    Serial.println("Dispositivo configurado como MESTRE");
#else
    Serial.println("Dispositivo configurado como ESCRAVO");
#endif

    // Configura o Wi-Fi no modo Station (necessário para ESP-NOW)
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();  // Não se conecta a nenhuma rede, uso só ESP-NOW

    // Inicializa ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("Erro ao inicializar ESP-NOW");
        return;
    }

    // Registra os callbacks de envio e recebimento
    esp_now_register_send_cb(onSend);
    esp_now_register_recv_cb(onReceive);

#ifndef AS_MASTER
    // Configura pino 2 como saída (no escravo) e inicia em nível baixo
    pinMode(2, OUTPUT);
    digitalWrite(2, LOW);

    // Adiciona peer de broadcast para permitir envio do handshake inicial
    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, broadcastAddress, 6);
    peerInfo.channel = 0;     // 0 = usa canal atual padrão
    peerInfo.encrypt = false;
    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Falha ao adicionar peer de broadcast");
    }

    // Envia pacote "hello" via broadcast para anunciar ao mestre
    Message helloMsg;
    helloMsg.magic  = MAGIC_WORD;
    helloMsg.opcode = OPCODE_HELLO;
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t*)&helloMsg, sizeof(helloMsg));
    if (result == ESP_OK) {
        Serial.println("Pacote de 'hello' enviado via broadcast");
    } else {
        Serial.println("Falha ao enviar pacote de 'hello'");
    }
#endif

}

void loop() {
#ifdef AS_MASTER
    // MESTRE: após o handshake, envia comandos de ativar/desativar para os escravos
    if (!sentActivation && slaveCount > 0) {
        // Se há pelo menos um escravo pareado, programa envio do comando de ativação 
        // 5 segundos após o primeiro pareamento
        if (firstPeerTime != 0 && millis() - firstPeerTime > 5000) {
            // Monta pacote opcode 3 (ATIVAR) e envia a todos os escravos registrados
            Message msg;
            msg.magic  = MAGIC_WORD;
            msg.opcode = OPCODE_ACTIVATE;
            Serial.println("Enviando comando ATIVAR (opcode 3) para todos os escravos");
            for (int i = 0; i < slaveCount; ++i) {
                esp_now_send(slaveAddresses[i], (uint8_t*)&msg, sizeof(msg));
            }
            sentActivation = true;
            activationTime = millis();
        }
    }
    if (sentActivation && !sentDeactivation) {
        // 30 segundos após ativação, envia comando de desativação
        if (millis() - activationTime > 30000) {
            // Monta pacote opcode 4 (DESATIVAR) e envia a todos os escravos registrados
            Message msg;
            msg.magic  = MAGIC_WORD;
            msg.opcode = OPCODE_DEACTIVATE;
            Serial.println("Enviando comando DESATIVAR (opcode 4) para todos os escravos");
            for (int i = 0; i < slaveCount; ++i) {
                esp_now_send(slaveAddresses[i], (uint8_t*)&msg, sizeof(msg));
            }
            sentDeactivation = true;
        }
    }
#else
    // ESCRAVO: nenhuma ação periódica necessária no loop (ações via callback de recebimento)
    // (Poderia re-enviar "hello" se não receber confirmação após um tempo, se necessário)
#endif
    delay(1000);  // Pequeno delay para evitar loop muito rápido (e facilitar leitura dos logs)
}
