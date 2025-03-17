#ifndef WEBSOCKET_HANDLER_H
#define WEBSOCKET_HANDLER_H

#include <ArduinoWebsockets.h>

typedef void (*OnWebSocketMessageCallback)(const char* event, const char* data);

void initWebSocket(const char* ws_url, OnWebSocketMessageCallback callback);
void sendWebSocketMessage(const char* event, const char* data);
void handleWebSocket();

#endif
