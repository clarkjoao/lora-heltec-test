#ifndef WEBSOCKET_HANDLER_H
#define WEBSOCKET_HANDLER_H

#include <ArduinoWebsockets.h>

typedef void (*OnWebSocketMessageCallback)(const char* message);

void initWebSocket(const char* ws_url, OnWebSocketMessageCallback callback);
void sendWebSocketMessage(const char* message);
void handleWebSocket();

#endif
