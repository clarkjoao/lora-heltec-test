#include "config.h"
#include "utils.h"

#ifdef DEVICE_MASTER
    #include "master_device.h"
    #include "wifi_manager.h"
    #include "websocket_handler.h"
#else
    #include "slave_device.h"
#endif

void onWebSocketMessage(const char* message)    {
    Serial.printf("Mensagem do servidor: %s\n", message);
}

void setup() {
    Serial.begin(115200);
    
#ifdef DEVICE_MASTER
    connectToWiFi(WIFI_SSID, WIFI_PASSWORD);
    initWebSocket(WS_URL, onWebSocketMessage);
    setupMaster();
#else
    setupSlave();
#endif
}

void loop() {
#ifdef DEVICE_MASTER
    handleWebSocket();
    loopMaster();
#else
    loopSlave();
#endif
}
