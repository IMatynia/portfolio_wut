#include "include/mapper.hpp"

Mapper::Mapper(AsyncIODevice *ioDevice, WallFollower *wallFollower, PositionTracker *positionTracker)
        : ioDevice(ioDevice), wallFollower(wallFollower), positionTracker(positionTracker) {

}

void Mapper::start() {
    this->running = true;
    printf("Starting mapper thread\n");
    this->mappingThread = riot::thread(mapperThreadHelper, this);
}

void Mapper::stop() {
    this->running = false;
    printf("Stopping mapper thread\n");
    this->mappingThread.join();
}

void Mapper::loop() {
    printf("Mapper loop start\n");
    MapperDataPointPacket* packet;
    while (this->running) {
        packet = this->preparePacket();
        std::string buffer = RobotComProtocolParser::serializePacket(packet);
        this->ioDevice->send(buffer);
        delete packet;
        xtimer_msleep(this->MAPPING_FREQUENCY);
    }
}

MapperDataPointPacket* Mapper::preparePacket() {
    return new MapperDataPointPacket(
            this->positionTracker->getX(),
            this->positionTracker->getY(),
            this->positionTracker->getAngle(),
            this->wallFollower->getDistanceRight()
            );
}

void *mapperThreadHelper(void *mapper_ptr) {
    printf("mapper thread pid: %d\n", thread_getpid());
    Mapper *mapper = (Mapper *) mapper_ptr;
    mapper->loop();
    return nullptr;
}
