#include "core/config.h"
#include "core/task_scheduler.h"
#include "drivers/wifi_manager.h"
#include "drivers/websocket_handler.h"
#include "modules/master_device.h"
#include <utils/utils.h>
#include <struct/slave.h>

TaskScheduler scheduler;

const int MAX_SLAVES = 20;
int slaveCount = 0;

Slave slaves[MAX_SLAVES];

int pingAttempts[MAX_SLAVES] = {0};

int findSlaveIndex(const uint8_t *mac) {
    for (int i = 0; i < MAX_SLAVES; i++) {
        if (slaves[i].macaddress == toHexString(mac, 6)) {
            return i;
        }
    }

    return -1;
}

void onMasterReceiveMessage(const uint8_t *mac, const Packet &msg) {
    int i = findSlaveIndex(mac);

    if (i == -1 && msg.opcode != REGISTER_SLAVE_REQ) {
        Serial.println("Slave not found in network");
        return;
    }

    switch (msg.opcode)
    {
        case REGISTER_SLAVE_REQ:
            Serial.println("New slave trying to register...");
            if(addPeerToNetwork(mac)){
                Serial.println("New slave added to network");

                Slave newSlave;
                newSlave.macaddress = toHexString(mac, 6);
                memcpy(newSlave.macAddr, mac, 6);
                newSlave.isConnected = true;
                newSlave.isWorking = false;
                newSlave.connectedAt = millis();
                newSlave.lastContact = millis();
                slaves[slaveCount] = newSlave;

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
        
        case PONG_ACK:
            slaves[i].isConnected = true;
            slaves[i].lastContact = millis();
            pingAttempts[i] = 0;
            break;

        default:
            break;
        }
}
  
void onWebSocketMessage(const char* event, const char* data) {
    Serial.printf("Received message from WebSocket: %s\n", event, data);
}


void sendPingToSlaves() {
    Serial.println("Sending PING to slaves...");

    for (int i = 0; i < slaveCount; i++) {
        if (slaves[i].isConnected) {
            Serial.printf("PING_REQ to %s (Attempt %d)\n", slaves[i].macaddress.c_str(), pingAttempts[i] + 1);

            sendESPNowMessage(slaves[i].macAddr, PING_REQ);
            pingAttempts[i]++;

            if (pingAttempts[i] >= 3) {
                Serial.printf("No response from %s, marking as disconnected.\n", slaves[i].macaddress.c_str());
                slaves[i].isConnected = false;
                pingAttempts[i] = 0;
            }
        }
    }
}

void registerTaskScheduler() {
    Serial.println("Registering task scheduler");

    // Periodically send list of slaves to the web interface
    scheduler.registerTask([]() {
        StaticJsonDocument<256> doc;
        JsonArray jsonArray = doc.createNestedArray("slaveAddresses");

        for (int i = 0; i < MAX_SLAVES; i++) {
            String mac = slaves[i].macaddress;
            
            if (mac == "00:00:00:00:00:00") {
                break;
            }
            jsonArray.add(mac);
        }

        String jsonString;
        serializeJson(doc, jsonString);

        sendWebSocketMessage(EventStrings.at(Events::SLAVES_LIST).c_str(), jsonString.c_str());
    }, 5000);// 5s;

    // Periodically send PING to slaves connected
    scheduler.registerTask([]() {
        sendPingToSlaves();
    }, 5000);  // Every 5s

}

void setupMaster() {
    // NOTE:
    // Espnow use the WiFi driver  2.4 ghz
    // If we connect to a 5ghz network, the espnow will not work
    connectToWiFi(WIFI_SSID, WIFI_PASSWORD);
    delay(100);
    initWebSocket(WS_URL, onWebSocketMessage);
    delay(100);
    initESPNow(onMasterReceiveMessage);
   registerTaskScheduler();
    Serial.println("Device initialized as Master");
}

void loopMaster() {
    handleWebSocket();
    scheduler.runTasks(); 
}
