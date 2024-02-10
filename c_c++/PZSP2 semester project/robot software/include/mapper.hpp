//
// Created by imat on 22.11.23.
//

#ifndef ROBOT_MAPUJACY_MAPPER_H
#define ROBOT_MAPUJACY_MAPPER_H

#include "wallFollower.hpp"
#include "IODevice.hpp"
#include "robotComParser.hpp"
#include "robotComProtol.hpp"
#include "positionTracker.hpp"
#include <riot/thread.hpp>
#include <xtimer.h>

class Mapper {
public:
    Mapper(AsyncIODevice* ioDevice, WallFollower* wallFollower, PositionTracker* positionTracker);
    void start();
    void stop();
private:
    void loop();
    MapperDataPointPacket* preparePacket();
    bool running = false;
    int MAPPING_FREQUENCY = 500;
    riot::thread mappingThread;

    AsyncIODevice* ioDevice;
    WallFollower* wallFollower;
    PositionTracker* positionTracker;

    friend void* mapperThreadHelper(void* mapper_ptr);
};

void* mapperThreadHelper(void* mapper_ptr);

#endif //ROBOT_MAPUJACY_MAPPER_H
