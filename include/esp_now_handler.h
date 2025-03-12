#ifndef ESP_NOW_HANDLER_H
#define ESP_NOW_HANDLER_H

#include <Arduino.h>
#include <esp_now.h>
#include "message.h"

extern uint8_t broadcastAddress[6];

typedef void (*OnReceiveCallback)(const uint8_t*, const Message&);

uint8_t addPeerToNetwork(const uint8_t *mac);
void removePeerFromNetwork(const uint8_t *mac);

void initESPNow(OnReceiveCallback callback);

void sendESPNowMessage(const uint8_t *mac, uint8_t opcode);

#endif
