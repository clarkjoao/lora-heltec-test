#include <Arduino.h>
#include <HTTPClient.h>
#include <esp_now.h>
#include <WiFi.h>


#define LED_PIN 2
#define LED2_PIN 16
#define BUZZER_PIN 18

void setup() {
  pinMode(LED_PIN, OUTPUT); // Configura o GPIO17 como saída
  pinMode(LED2_PIN, OUTPUT); 
  pinMode(BUZZER_PIN, OUTPUT);
}

void loop() {
    digitalWrite(LED_PIN, HIGH);
    digitalWrite(LED2_PIN, HIGH);
  // Sobe a frequência de 100Hz até 2000Hz
  for (int freq = 100; freq <= 2000; freq += 50) {
    tone(BUZZER_PIN, freq);
    delay(50);
  }
  
  // Desce a frequência de 2000Hz até 100Hz
  for (int freq = 2000; freq >= 100; freq -= 50) {
    tone(BUZZER_PIN, freq);
    delay(50);
  }
  
  noTone(BUZZER_PIN);
  digitalWrite(LED2_PIN, LOW);
  digitalWrite(LED_PIN, LOW);
  delay(5000);
}
