#include <Arduino.h>
#include "Heltec.h"
#include "esp_system.h"
#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "Casa";
const char* password = "Margarida";
const char* serverUrl = "http://192.168.2.95:3000/data";
void setup() {
  Serial.begin(115200);
  Heltec.begin(true, false, true);
  
  Serial.print("Conectando ao Wi-Fi...");
  WiFi.begin(ssid, password);
  int retry_count = 0;
  while (WiFi.status() != WL_CONNECTED && retry_count < 10) {
      delay(1000);
      Serial.print(".");
      retry_count++;
  }
  
  Heltec.display->clear();
  Heltec.display->setTextAlignment(TEXT_ALIGN_CENTER);
  Heltec.display->setFont(ArialMT_Plain_16);
  
  if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\nWi-Fi conectado!");
      Heltec.display->drawString(64, 22, "Wi-Fi Conectado!");
  } else {
      Serial.println("\nFalha ao conectar Wi-Fi");
      Heltec.display->drawString(64, 22, "Wi-Fi NAO encontrado!");
  }
  
  Heltec.display->display();
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
      // Obtém o MAC Address do ESP32
      uint8_t mac[6];
      esp_read_mac(mac, ESP_MAC_WIFI_STA);
      char macStr[18];
      snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X", 
               mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
      
      // Criar JSON
      String jsonPayload = "{\"mac\":\"" + String(macStr) + "\", \"data\":\"OI JOAO\"}";
      Serial.println("Enviando JSON: " + jsonPayload);
      
      // Faz requisição ao servidor
      HTTPClient http;
      http.begin(serverUrl);
      http.addHeader("Content-Type", "application/json");
      int httpCode = http.POST(jsonPayload);
      
      Heltec.display->clear();
      Heltec.display->setTextAlignment(TEXT_ALIGN_CENTER);
      Heltec.display->setFont(ArialMT_Plain_16);
      Heltec.display->drawString(64, 22, "Enviando...");
      Heltec.display->display();
      
      if (httpCode > 0) {
          Serial.println("Dados enviados com sucesso!");
          String response = http.getString();
          Serial.println("Resposta do servidor: " + response);
          Heltec.display->drawString(64, 42, "Enviado!");
      } else {
          Serial.println("Erro ao enviar os dados");
          Heltec.display->drawString(64, 42, "Erro no envio");
      }
      http.end();
      
      Heltec.display->display();
  }
  delay(5000); // Aguarda 5 segundos antes de enviar novamente
}
