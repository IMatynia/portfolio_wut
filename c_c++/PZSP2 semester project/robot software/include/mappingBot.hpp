/*
 * Development during 23Z semester by
 * I. Matynia
 */

#ifndef INCLUDE_MAPPINGBOT_HPP_
#define INCLUDE_MAPPINGBOT_HPP_

#include "bluetooth.hpp"
#include "robotComParser.hpp"
#include "wallFollower.hpp"
#include "positionTracker.hpp"
#include "mapper.hpp"
#include <riot/mutex.hpp>

class CommandHandler;

enum robotState {
    INIT,
    WAITING_FOR_CONNECTION,
    PAUSED,
    RUNNING,
};

class MappingBot {
public:
    MappingBot(BluetoothIO* bluetooth, WallFollower* wallFollower, PositionTracker* positionTracker, Mapper* mapper, gpio_t button_pin);
    ~MappingBot();
    void start();
    void runMainLoop();
    void stopMainLoop();
private:
    robotState state = INIT;
    riot::mutex state_change_mutex;

    BluetoothIO* bluetooth;
    CommandHandler* commandHandler;

    WallFollower* wallFollower;
    PositionTracker* positionTracker;
    Mapper* mapper;

    gpio_t button_pin;
    char* buttonCallbackThreadStack;
    void handleUserCommands(const std::string& buffer);
    void handleUserButtonPress();
    int initButton();
    IODataCallback_t handleUserCommandsCallback;

    friend void buttonCallbackHelper(void* arg);
    friend void* buttonThreadTaskHelper(void* arg);
    friend class CommandHandler;
};

void buttonCallbackHelper(void* arg);
void* buttonThreadTaskHelper(void *mappingBot_ptr);

#endif