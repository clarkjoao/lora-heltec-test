#ifndef MESSAGE_H
#define MESSAGE_H

#include "config.h"

enum Opcodes {
    REGISTER_SLAVE_REQ = 0x01,
    REGISTER_SLAVE_ACK = 0x02,
    ACTIVATE_PIN_REQ = 0x03,
    ACTIVATE_PIN_ACK = 0x04,
    DEACTIVATE_PIN_REQ = 0x05,
    DEACTIVATE_PIN_ACK = 0x06,
};

typedef struct Message {
    uint16_t magicByte;
    uint8_t opcode;
    uint8_t macAddr[6];
    time_t timestamp;
} Message;

#endif
