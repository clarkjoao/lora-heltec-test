#include "config.h"
#include "utils.h"

#ifdef DEVICE_MASTER
    #include "master_device.h"
#else
    #include "slave_device.h"
#endif

void setup() {
    Serial.begin(115200);
    
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
