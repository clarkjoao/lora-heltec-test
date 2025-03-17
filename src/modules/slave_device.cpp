#include <iostream>
#include "config.h"
#include "modules/slave_device.h"
#include <esp_wifi.h>

uint8_t masterAddress[6];
bool masterRegistered = false;

struct PinConfig {
    static constexpr int LED_PIN = 2;
    static constexpr int SWITCH_PIN = 16;
};

void setupPinMode() {
    pinMode(PinConfig::LED_PIN, OUTPUT);
    digitalWrite(PinConfig::LED_PIN, LOW);
    
    pinMode(PinConfig::SWITCH_PIN, OUTPUT);
    digitalWrite(PinConfig::SWITCH_PIN, LOW);
}

int currentChannel = 1;  // Começa no canal 1
const int MAX_CHANNEL = 13;
int registerAttempts = 0;

void changeChannel() {
    currentChannel++;
    if (currentChannel > MAX_CHANNEL) {
        currentChannel = 1;  // Volta para o canal 1 após 13
    }
    Serial.printf("Changing to channel %d\n", currentChannel);
    esp_wifi_set_channel(currentChannel, WIFI_SECOND_CHAN_NONE);
}

void tryRegisterMaster() {
    if (masterRegistered) return;

    Serial.printf("Attempt %d to register on channel %d\n", registerAttempts + 1, currentChannel);
    sendESPNowMessage(broadcastAddress, REGISTER_SLAVE_REQ);
    
    registerAttempts++;

    if (registerAttempts >= 3) {  
        changeChannel();  // Muda de canal após 3 falhas
        registerAttempts = 0;  // Reseta a contagem
    }

    delay(500);  // Espera 500ms antes da próxima tentativa
}

void onSlaveReceiveMessage(const uint8_t *mac, const Packet &msg) {
    switch (msg.opcode)
    {
        case REGISTER_SLAVE_ACK:
            if(addPeerToNetwork(mac)){
                removePeerFromNetwork(broadcastAddress);
                Serial.println("Master registered");
                memcpy(masterAddress, mac, 6);
                masterRegistered = true;
            } else {
                Serial.println("Failed to add master to network");
            }
            break;

        case ACTIVATE_PIN_REQ:
            Serial.println("Activate command received from master. Enabling pin 2.");
            digitalWrite(PinConfig::LED_PIN, HIGH);
            digitalWrite(PinConfig::SWITCH_PIN, HIGH);
            sendESPNowMessage(masterAddress, ACTIVATE_PIN_ACK);
            break;

        case DEACTIVATE_PIN_REQ:
            Serial.println("Deactivate command received from master. Disabling pin 2.");
            digitalWrite(PinConfig::LED_PIN, LOW);
            digitalWrite(PinConfig::SWITCH_PIN, LOW);
            sendESPNowMessage(masterAddress, DEACTIVATE_PIN_ACK);
            break;
        
        case PING_REQ:
            Serial.println("PING_REQ received from master");
            sendESPNowMessage(masterAddress, PONG_ACK);
            Serial.println("PONG_ACK sent to master");
            break;

        default:
            break;
    }
}

void setupSlave() {
    setupPinMode();
    initESPNow(onSlaveReceiveMessage);
    Serial.println("Device initialized as Slave");
}

void loopSlave() {
    tryRegisterMaster();
}
