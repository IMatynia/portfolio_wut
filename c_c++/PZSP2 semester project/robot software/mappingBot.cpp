/*
 * Development during 23Z semester by
 * I. Matynia
 */

#include "include/mappingBot.hpp"
#include "include/commandHandler.hpp"
#include "xtimer.h"

MappingBot::MappingBot(BluetoothIO* bluetooth, WallFollower* wallFollower, PositionTracker* positionTracker, Mapper* mapper, gpio_t button_pin): bluetooth(bluetooth), wallFollower(wallFollower), positionTracker(positionTracker), mapper(mapper), button_pin(button_pin) {
    handleUserCommandsCallback = std::bind(&MappingBot::handleUserCommands, this, std::placeholders::_1);
    commandHandler = new CommandHandler(this);
    buttonCallbackThreadStack = new char[THREAD_STACKSIZE_MAIN];
}

MappingBot::~MappingBot(){
    delete[] buttonCallbackThreadStack;
    delete commandHandler;
}

void MappingBot::start() {
    riot::lock_guard<riot::mutex> guard(this->state_change_mutex);
    if(this->state != INIT)
    {
        printf("Robot has already been initialized\n");
        return;
    }
    bluetooth->open();
    this->state = WAITING_FOR_CONNECTION;
    bluetooth->addDataReceivedCallback(&this->handleUserCommandsCallback);

    int result = initButton();
    if (result == 0) printf("Button hooked succesfully\n");
    else printf("ERROR: couldn't hook button\n");
}

int MappingBot::initButton() {
    int result;
    result = gpio_init_int(BTN0_PIN, GPIO_IN_PU, GPIO_RISING, buttonCallbackHelper, this);

    if (result != 0) return result;
    gpio_irq_enable(BTN0_PIN);

    return result;
}

void MappingBot::runMainLoop() {
    riot::lock_guard<riot::mutex> guard(this->state_change_mutex);
    if(this->state == RUNNING) return;
    this->state = RUNNING;
    printf("Main robot loop start\n");
    wallFollower->start();
    positionTracker->start();
    mapper->start();
}

void MappingBot::stopMainLoop() {
    riot::lock_guard<riot::mutex> guard(this->state_change_mutex);
    if(this->state == PAUSED) return;
    this->state = PAUSED;
    printf("Main robot loop stop\n");
    wallFollower->stop();
    positionTracker->stop();
    mapper->stop();
    xtimer_sleep(2);
    printf("Loop stopped\n");
}

void MappingBot::handleUserCommands(const std::string& buffer) {
    RobotComPacket* packet = RobotComProtocolParser::parsePacketString(buffer);
    if(packet == nullptr) return;
    packet->accept(this->commandHandler);
    delete packet;
}

void MappingBot::handleUserButtonPress() {
    switch (this->state) {
        case WAITING_FOR_CONNECTION:
            if (this->bluetooth->getBluetoothStatus() == BT_CONNECTED) {
                this->runMainLoop();
            } else {
                printf("Can't start the mapping yet, the robot is waiting for connection\n");
                // TODO: UX: send warning flashing lights
            }
            break;
        case PAUSED:
            this->runMainLoop();
            break;

        case RUNNING:
            this->stopMainLoop();
            break;

        default:
            printf("Can't handle this button press\n");
            break;
    }
}

void* buttonThreadTaskHelper(void *mappingBot_ptr)
{
    MappingBot* bot = (MappingBot*)mappingBot_ptr;
    bot->handleUserButtonPress();
    return nullptr;
}

void buttonCallbackHelper(void *arg)
{
    MappingBot* bot = (MappingBot*)arg;
    thread_create(bot->buttonCallbackThreadStack, THREAD_STACKSIZE_MAIN,
                  THREAD_PRIORITY_MAIN - 3, THREAD_CREATE_STACKTEST,
                  buttonThreadTaskHelper, arg, "buttonHelper");
    printf("button callback thread pid: %d\n", thread_getpid());
}
//TODO: reorganize thread priorities