//
// Created by imat on 24.11.23.
//

#ifndef ROBOT_MAPUJACY_ROBOTCONTEXT_H
#define ROBOT_MAPUJACY_ROBOTCONTEXT_H

#include "bluetooth.hpp"
#include "devicesConfig.hpp"
#include "dualMotorController.hpp"
#include "magnetometer.hpp"
#include "mapper.hpp"
#include "mappingBot.hpp"
#include "motorDriver.hpp"
#include "positionTracker.hpp"
#include "pulserDriver.hpp"
#include "ultrasonicSensorDriver.hpp"
#include "wallFollower.hpp"

MotorDriver* motorR = nullptr;
MotorDriver* motorL = nullptr;
PulserDriver* pulserR = nullptr;
PulserDriver* pulserL = nullptr;
UltrasonicSensorDriver* ultrasonicSensorL = nullptr;
UltrasonicSensorDriver* ultrasonicSensorR = nullptr;
UltrasonicSensorDriver* ultrasonicSensorM = nullptr;
DualMotorController* motController = nullptr;
WallFollower* wallFollower = nullptr;
PositionTracker* posTracker = nullptr;
BluetoothIO* bluetooth = nullptr;
Mapper* mapper = nullptr;
Magnetometer *magnetometer = nullptr;
MappingBot* bot = nullptr;

void init_hardware()
{
    gpio_init(LED0_PIN, GPIO_OUT);
    gpio_init(devicesConfig::bluetooth.resetPin, GPIO_OUT);

    motorR = new MotorDriver(&(devicesConfig::motorRight));
    motorL = new MotorDriver(&(devicesConfig::motorLeft));
    pulserR = new PulserDriver(&(devicesConfig::pulserRight));
    pulserL = new PulserDriver(&(devicesConfig::pulserLeft));
    ultrasonicSensorL = new UltrasonicSensorDriver(&(devicesConfig::ultrasonicSensorLeft));
    ultrasonicSensorR = new UltrasonicSensorDriver(&(devicesConfig::ultrasonicSensorRight));
    ultrasonicSensorM = new UltrasonicSensorDriver(&(devicesConfig::ultrasonicSensorMiddle));
    motController = new DualMotorController(motorR, motorL, pulserL, pulserR, 20);
    magnetometer = new Magnetometer();
    bluetooth = new BluetoothIO(devicesConfig::bluetooth.device, devicesConfig::bluetooth.baud);
}

void init_high_level_divers()
{
    wallFollower = new WallFollower(motController, ultrasonicSensorM, ultrasonicSensorR);
    posTracker = new PositionTracker(magnetometer, pulserL, pulserR);
    mapper = new Mapper(bluetooth, wallFollower, posTracker);
}

void compose_robot()
{
    bot = new MappingBot(bluetooth, wallFollower, posTracker, mapper, BTN0_PIN);
}

void* button_thread_helper(void*)
{
    init_hardware();
    init_high_level_divers();
    compose_robot();
    printf("Robot starting thread created\n");
    bot->start();
    return nullptr;
}

char stack[THREAD_STACKSIZE_MAIN];
bool initialized = false;

void button_helper(void*)
{
    if(initialized) return;
    printf("Button pressed\n");
    thread_create(stack, THREAD_STACKSIZE_MAIN,
                  THREAD_PRIORITY_MAIN - 1, THREAD_CREATE_STACKTEST,
                  button_thread_helper, nullptr, "initialButtonHelper");
    initialized = true;
}

void initial_button_setup()
{
    int result = gpio_init_int(BTN0_PIN, GPIO_IN_PU, GPIO_RISING, button_helper, nullptr);
    gpio_irq_enable(BTN0_PIN);
    if(result == 0)
        printf("Initial button hook set up\n");
    else
        printf("Error initializing initial button hook\n");
}

#endif //ROBOT_MAPUJACY_ROBOTCONTEXT_H
