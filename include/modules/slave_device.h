#ifndef SLAVE_DEVICE_H
#define SLAVE_DEVICE_H

#include "drivers/esp_now_handler.h"

void setupPinMode();
void setupSlave();
void loopSlave();

#endif
