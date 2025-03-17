#include "drivers/websocket_handler.h"
#include <WiFi.h>
#include <ArduinoWebsockets.h>
#include "core/config.h"
#include <ArduinoJson.h> 

using namespace websockets;

WebsocketsClient client;
OnWebSocketMessageCallback messageCallback = nullptr;
bool isConnected = false;

void onMessageCallback(WebsocketsMessage message) {
    if (message.length() < 3) {
        return;
    }

    String jsonData = message.data().substring(2);

    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, jsonData);

    if (error) {
        return;
    }

    const char* event = doc["event"];
    JsonVariant data = doc["data"];

    if (!event || data.isNull()) {
        return;
    }

    serializeJson(data, Serial);
    Serial.println();

    if (messageCallback) {
        messageCallback(event, data.as<String>().c_str());
    }
}

void onEventCallback(WebsocketsEvent event, String data) {
    switch (event) {
        case WebsocketsEvent::ConnectionOpened:
            Serial.println("WebSocket Connected!");
            isConnected = true;
            break;
        case WebsocketsEvent::ConnectionClosed:
            Serial.println("WebSocket Disconnected.");
            isConnected = false;
            break;
        case WebsocketsEvent::GotPing:
            Serial.println("Received Ping from Server.");
            break;
        
        case WebsocketsEvent::GotPong:
            Serial.println("Received Pong response.");
            break;
    }
}

void initWebSocket(const char* ws_url, OnWebSocketMessageCallback callback) {
    Serial.printf("Websocket Connected %s:%d...\n", ws_url);

    client.onMessage(onMessageCallback);
    client.onEvent(onEventCallback);

    if (client.connect(ws_url)) {
        Serial.println("Connected to WebSocket!");
    } else {
        Serial.println("Failed to connect to WebSocket!");
    }

    messageCallback = callback;
}

void sendWebSocketMessage(const char* event, const char* data) {
    if (!isConnected) {
        Serial.println("Not connected to WebSocket!");
        return;
    }

    StaticJsonDocument<256> doc;
    
    doc["event"] = event;
    doc["data"] = data;

    String jsonMessage;
    serializeJson(doc, jsonMessage);

    Serial.printf("Sending WebSocket message: %s\n", jsonMessage.c_str());
    client.send(jsonMessage);
}


void handleWebSocket() {
    if (client.available()) {
        client.poll();
    }
}
