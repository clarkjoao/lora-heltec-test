#include "utils/utils.h"

void printMac(const uint8_t *mac) {
    char macStr[18];
    snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    Serial.print(macStr);
}


String stringify(const JsonDocument& doc) {
    String jsonString;
    serializeJson(doc, jsonString);
    return jsonString;
}

bool parseJson(const String& jsonString, JsonDocument& doc) {
    DeserializationError erro = deserializeJson(doc, jsonString);
    return !erro;
}

String toHexString(const uint8_t* mac, size_t length) {
    String macStr;
    for (size_t i = 0; i < length; i++) {
        if (i > 0) macStr += ":";
        if (mac[i] < 0x10) macStr += "0";
        macStr += String(mac[i], HEX);
    }
    return macStr;
}

bool toByteArray(const String& macStr, uint8_t* macArray, size_t length) {
    if (macStr.length() != 17) return false;
    
    char buffer[3] = {0};
    int macIndex = 0;

    for (int i = 0; i < macStr.length(); i += 3) {
        if (macIndex >= length) return false;

        buffer[0] = macStr[i];     
        buffer[1] = macStr[i + 1]; 
        buffer[2] = '\0';          

        macArray[macIndex++] = strtoul(buffer, nullptr, 16);
    }

    return true;
}