#include "drivers/esp_now_handler.h"
#include <WiFi.h>
#include <esp_now.h>
#include "utils/utils.h"

uint8_t broadcastAddress[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

OnReceiveCallback recvCallback = nullptr;

uint8_t addPeerToNetwork(const uint8_t *mac) {
    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, mac, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) == ESP_OK) {
        return 1;
    } else {
        Serial.println("Error at adding peer.");
        return 0;
    }
}

void removePeerFromNetwork(const uint8_t *mac) {
    if (esp_now_del_peer(mac) == ESP_OK) {
        Serial.println("Peer removed.");
    } else {
        Serial.println("Erro ao remover peer.");
    }
}

void onSend(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.print("Sending status: ");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Error");
}

void onReceive(const uint8_t *mac, const uint8_t *incomingData, int len) {
    if (len < sizeof(Packet)) {
        Serial.println("Invalid packet size, from: ");
        printMac(mac);
        return;
    }

    Packet pkt;
    memcpy(&pkt, incomingData, sizeof(pkt));

    if (pkt.magicByte != MAGIC_WORD){
        Serial.println("Invalid magic word from:");
        printMac(mac);
        Serial.println("Ignoring packet");
        Serial.println(pkt.opcode);
        return;
    }

    Serial.print("Opcode Received: ");
    Serial.println(pkt.opcode);

    if (recvCallback != nullptr) {
        recvCallback(mac, pkt);
    }
}

void initESPNow(OnReceiveCallback callback) {

    // wifi_mode_t currentMode;
    // esp_wifi_get_mode(&currentMode);

    // if (currentMode != WIFI_MODE_STA) {
    //     Serial.println("Changing Wi-Fi mode to STA...");
   // WiFi.mode(WIFI_STA);
    // }

    if (esp_now_init() != ESP_OK) {
        Serial.println("Error at started ESP-NOW");
    }
    
    esp_now_register_send_cb(onSend);
    esp_now_register_recv_cb(onReceive);

    if(addPeerToNetwork(broadcastAddress)){
        Serial.println("Broadcast peer added");
    }

    recvCallback = callback;
}

void sendESPNowMessage(const uint8_t *mac, uint8_t opcode) {
    Packet pkt = {MAGIC_WORD, opcode};
    esp_now_send(mac, (uint8_t*)&pkt, sizeof(pkt));
    Serial.print("Sending message to: ");
    printMac(mac);
    Serial.print(" with opcode: ");
    Serial.println(opcode);
}
