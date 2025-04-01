#include <WiFi.h>
#include <ArduinoWebsockets.h>

using namespace websockets;

const char* ssid = "Miau";
const char* password = "7gafanhoto*b";
const char* websocket_server = "ws://192.168.86.169:3001";

WebsocketsClient client;

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);

    Serial.print("Conectando ao Wi-Fi...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWi-Fi conectado!");

    Serial.println("Conectando ao WebSocket...");
    if (client.connect(websocket_server)) {
        Serial.println("Conectado ao WebSocket!");
        client.send("ESP32 conectado!");
    } else {
        Serial.println("Falha ao conectar ao WebSocket.");
    }
}

void loop() {
    if (client.available()) {
        client.poll();
    }
}
