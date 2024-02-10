//
// Created by imat on 22.11.23.
//

#ifndef ROBOT_MAPUJACY_VISITOR_H
#define ROBOT_MAPUJACY_VISITOR_H

class RunMainLoopCommandPacket;
class StopMainLoopCommandPacket;
class MapperDataPointPacket;

class AbstractPacketHandler{
public:
    virtual ~AbstractPacketHandler() {};
    virtual void handle(RunMainLoopCommandPacket* packet) = 0;
    virtual void handle(StopMainLoopCommandPacket* packet) = 0;
    virtual void handle(MapperDataPointPacket* packet) = 0;
};

#endif //ROBOT_MAPUJACY_VISITOR_H
