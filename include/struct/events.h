#ifndef EVENTS_H
#define EVENTS_H

#include "core/config.h"
#include <unordered_map>
#include <string>

enum class Events {
    REGISTER_MASTER,
    REGISTER_SLAVE,
    UNREGISTER_SLAVE,
    UNREGISTER_MASTER,
    SLAVES_LIST,
    SLAVE_ACTIVATED,
    SLAVE_DEACTIVATED,
    SLAVE_STATE
};

const std::unordered_map<Events, std::string> EventStrings = {
    {Events::REGISTER_MASTER, "REGISTER_MASTER"},
    {Events::REGISTER_SLAVE, "REGISTER_SLAVE"},
    {Events::UNREGISTER_SLAVE, "UNREGISTER_SLAVE"},
    {Events::UNREGISTER_MASTER, "UNREGISTER_MASTER"},
    {Events::SLAVES_LIST, "SLAVES_LIST"},
    {Events::SLAVE_ACTIVATED, "SLAVE_ACTIVATED"},
    {Events::SLAVE_DEACTIVATED, "SLAVE_DEACTIVATED"},
    {Events::SLAVE_STATE, "SLAVE_STATE"}
};

typedef struct Event {
    Events event;
    std::string data;
    time_t timestamp;
} Event;

#endif
