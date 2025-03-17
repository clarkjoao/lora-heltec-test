#include "core/config.h"
#include "utils/utils.h"
#include <WiFi.h>

#ifdef DEVICE_MASTER
    #include "modules/master_device.h"
#else
    #include "modules/slave_device.h"
#endif

void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_AP_STA);
    
#ifdef DEVICE_MASTER
    setupMaster();
#else
    setupSlave();
#endif
}

void loop() {
#ifdef DEVICE_MASTER
    loopMaster();
#else
    loopSlave();
#endif
}
