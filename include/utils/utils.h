#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>

void printMac(const uint8_t *mac);


#include <ArduinoJson.h>
#include <Arduino.h>

String stringify(const JsonDocument& doc);

bool parseJson(const String& jsonString, JsonDocument& doc);

String toHexString(const uint8_t* mac, size_t length);
bool toByteArray(const String& macStr, uint8_t* macArray, size_t length);

#endif // UTILS_H
