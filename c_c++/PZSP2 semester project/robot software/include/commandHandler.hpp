//
// Created by imat on 22.11.23.
//

#ifndef ROBOT_MAPUJACY_COMMANDHANDLER_H
#define ROBOT_MAPUJACY_COMMANDHANDLER_H

#include "abstractPacketHandler.hpp"
#include "robotComProtol.hpp"

class MappingBot;

class CommandHandler: public AbstractPacketHandler
{
public:
    CommandHandler(MappingBot* mappingBot): mappingBot(mappingBot) {};
    void handle(RunMainLoopCommandPacket* packet);
    void handle(StopMainLoopCommandPacket* packet);
    void handle(MapperDataPointPacket*) {};
private:
    MappingBot* mappingBot;
};

#endif //ROBOT_MAPUJACY_COMMANDHANDLER_H
