#include "esp_now_handler.h"
#include <WiFi.h>
#include <esp_now.h>
#include "utils.h"

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
        Serial.println("Erro ao adicionar peer.");
        return 0;
    }
}

void removePeerFromNetwork(const uint8_t *mac) {
    if (esp_now_del_peer(mac) == ESP_OK) {
        Serial.println("Peer removido com sucesso.");
    } else {
        Serial.println("Erro ao remover peer.");
    }
}

void onSend(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.print("Sending status: ");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Error");
}

void onReceive(const uint8_t *mac, const uint8_t *incomingData, int len) {
    if (len < sizeof(Message)) {
        Serial.println("Invalid packet size, from: ");
        printMac(mac);
        return;
    }

    Message msg;
    memcpy(&msg, incomingData, sizeof(msg));

    if (msg.magicByte != MAGIC_WORD){
        Serial.println("Invalid magic word from:");
        printMac(mac);
        Serial.println("Ignoring packet");
        Serial.println(msg.opcode);
        return;
    }

    Serial.print("Opcode Received: ");
    Serial.println(msg.opcode);

    if (recvCallback != nullptr) {
        recvCallback(mac, msg);
    }
}

void initESPNow(OnReceiveCallback callback) {
    WiFi.mode(WIFI_STA);

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
    Message msg = {MAGIC_WORD, opcode};
    esp_now_send(mac, (uint8_t*)&msg, sizeof(msg));
    Serial.print("Sending message to: ");
    printMac(mac);
    Serial.print(" with opcode: ");
    Serial.println(opcode);
}
