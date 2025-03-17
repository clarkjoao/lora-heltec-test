#ifndef SLAVE_H
#define SLAVE_H

#include <Arduino.h>

struct Slave {
    String macaddress;
    uint8_t macAddr[6];
    bool isConnected;
    bool isWorking;
    unsigned long connectedAt;
    unsigned long lastContact;

    Slave() : macaddress(""), isConnected(false), isWorking(false), connectedAt(0), lastContact(0) {}

    Slave(String mac, bool connected, bool working, unsigned long connectedAt, unsigned long lastContact)
        : macaddress(mac), isConnected(connected), isWorking(working), connectedAt(connectedAt), lastContact(lastContact) {}

};

#endif // SLAVE_H
