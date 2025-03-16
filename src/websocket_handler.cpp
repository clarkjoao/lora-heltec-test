#include "websocket_handler.h"
#include <WiFi.h>
#include <ArduinoWebsockets.h>

using namespace websockets;

WebsocketsClient client;
OnWebSocketMessageCallback messageCallback = nullptr;
bool isConnected = false;

void onMessageCallback(WebsocketsMessage message) {
    Serial.printf("Mensagem recebida: %s\n", message.data().c_str());
    if (messageCallback) {
        messageCallback(message.data().c_str());
    }
}

void onEventCallback(WebsocketsEvent event, String data) {
    switch (event) {
        case WebsocketsEvent::ConnectionOpened:
            Serial.println("WebSocket conectado!");
            isConnected = true;
            client.send("Olá, servidor!");
            break;
        case WebsocketsEvent::ConnectionClosed:
            Serial.println("WebSocket desconectado.");
            isConnected = false;
            break;
        case WebsocketsEvent::GotPing:
            Serial.println("Ping recebido.");
            break;
        case WebsocketsEvent::GotPong:
            Serial.println("Pong recebido.");
            break;
    }
}

void initWebSocket(const char* ws_url, OnWebSocketMessageCallback callback) {
    Serial.printf("Conectando ao WebSocket em %s:%d...\n", ws_url);

    client.onMessage(onMessageCallback);
    client.onEvent(onEventCallback);

    if (client.connect(ws_url)) {
        Serial.println("Conexão bem-sucedida!");
    } else {
        Serial.println("Falha na conexão com WebSocket.");
    }

    messageCallback = callback;
}

void sendWebSocketMessage(const char* message) {
    if (isConnected) {
        Serial.printf("Enviando mensagem: %s\n", message);
        client.send(message);
    } else {
        Serial.println("Erro: WebSocket não está conectado!");
    }
}

void handleWebSocket() {
    if (client.available()) {
        client.poll();
    }
}
