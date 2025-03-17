#include "drivers/wifi_manager.h"
#include <WiFi.h>

void connectToWiFi(const char* ssid, const char* password) {
    WiFi.begin(ssid, password);
    Serial.print("Trying to connect to ");
    Serial.print(ssid);

    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(1000);
    }

    Serial.println();
    Serial.println("Connected to WiFi");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
}
