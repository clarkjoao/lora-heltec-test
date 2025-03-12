#include "config.h"
#include "master_device.h"

const int MAX_SLAVES = 20;
uint8_t slaveAddresses[MAX_SLAVES][6];
int slaveCount = 0;
bool sentActivation = false;

void onMasterReceiveMessage(const uint8_t *mac, const Message &msg) {

    switch (msg.opcode)
    {
        case REGISTER_SLAVE_REQ:
            Serial.println("New slave trying to register...");
            if(addPeerToNetwork(mac)){
                Serial.println("New slave added to network");
                memcpy(slaveAddresses[slaveCount], mac, 6);
                slaveCount++;
                sendESPNowMessage(mac, REGISTER_SLAVE_ACK);
            }
            break;
        
        case ACTIVATE_PIN_ACK:
            Serial.println("Slave activated");
            break;
        
        case DEACTIVATE_PIN_ACK:
            Serial.println("Slave deactivated");
            break;

        default:
            break;
        }
}
  


void setupMaster() {
    initESPNow(onMasterReceiveMessage);
    Serial.println("Dispositivo configurado como MESTRE");
}

void loopMaster() {

    if (!sentActivation) {
        for (int i = 0; i < slaveCount; i++) {
            sendESPNowMessage(slaveAddresses[i], ACTIVATE_PIN_REQ);
        }
        sentActivation = true;
    }else{
        for (int i = 0; i < slaveCount; i++) {
            sendESPNowMessage(slaveAddresses[i], DEACTIVATE_PIN_REQ);
        }
        sentActivation = false;
    }
    delay(2000);

}
