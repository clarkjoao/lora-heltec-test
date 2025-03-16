#include "core/config.h"
#include "utils/utils.h"

#ifdef DEVICE_MASTER
    #include "drivers/wifi_manager.h"
    #include "drivers/websocket_handler.h"
    #include "modules/master_device.h"
#else
    #include "modules/slave_device.h"
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
