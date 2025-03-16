#include <iostream>
#include "config.h"
#include "modules/slave_device.h"

uint8_t masterAddress[6];
bool masterRegistered = false;

struct PinConfig {
    static constexpr int LED_PIN = 2; // ESP32 GPIO2
    static constexpr int SWITCH_PIN = 16; // Enabling GPIO16 as output
};

void setupPinMode() {
    pinMode(PinConfig::LED_PIN, OUTPUT);
    digitalWrite(PinConfig::LED_PIN, LOW);
    
    pinMode(PinConfig::SWITCH_PIN, OUTPUT);
    digitalWrite(PinConfig::SWITCH_PIN, LOW);
}

void onSlaveReceiveMessage(const uint8_t *mac, const Message &msg) {

    switch (msg.opcode)
    {
        case REGISTER_SLAVE_ACK:
            if(addPeerToNetwork(mac)){
                removePeerFromNetwork(broadcastAddress);
                Serial.println("Master registered");
                masterAddress[6] = mac[6];
                masterRegistered = true;
            }else{
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
        
        default:
            break;
        }
}

void setupSlave() {
    Serial.println("Dispositivo configurado como ESCRAVO");
    setupPinMode();
    
    initESPNow(onSlaveReceiveMessage);
    }

void loopSlave() {
    if(masterRegistered) return;
    sendESPNowMessage(broadcastAddress, REGISTER_SLAVE_REQ);
    delay(1000);
    
}
