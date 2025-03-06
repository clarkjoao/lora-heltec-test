#include <Wire.h>
#include <SPI.h>
#include "Heltec.h"

#define LORA_BAND 433E6  // Change based on your region: 433E6, 868E6, 915E6

void setup() {
    Serial.begin(115200);
    while (!Serial);

    // Initialize Heltec Board (OLED + LoRa)
    Heltec.begin(true /* Display */, true /* LoRa */, true /* Serial */, true /* PABOOST */, LORA_BAND);

    Serial.println("Heltec WiFi LoRa 32 V3 Initialized!");
    
    // Display Initial Message
    Heltec.display->clear();
    Heltec.display->setTextAlignment(TEXT_ALIGN_CENTER);
    Heltec.display->setFont(ArialMT_Plain_10);
    Heltec.display->drawString(64, 10, "LoRa Initialized!");
    Heltec.display->display();
    
    Serial.println("LoRa Ready!");
}

void loop() {
    Serial.println("Sending LoRa Message...");
    
    LoRa.beginPacket();
    LoRa.print("Hello from Heltec V3!");
    LoRa.endPacket();

    // Update OLED Display
    Heltec.display->clear();
    Heltec.display->drawString(64, 10, "Message Sent!");
    Heltec.display->drawString(64, 25, "LoRa: Hello from V3");
    Heltec.display->display();

    delay(5000); // Wait 5 seconds before sending again
}
