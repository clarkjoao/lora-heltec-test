#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>

// Definir como MESTRE ou ESCRAVO
#ifdef AS_MASTER
    #define DEVICE_MASTER 1
#endif

#define WIFI_CHANNEL 3

const uint16_t MAGIC_WORD = 0xCAFE;

#endif
