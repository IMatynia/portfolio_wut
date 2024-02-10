#include "include/commandHandler.hpp"
#include "include/mappingBot.hpp"

void CommandHandler::handle(RunMainLoopCommandPacket*)
{
    if(this->mappingBot->state == PAUSED || this->mappingBot->state == WAITING_FOR_CONNECTION)
        this->mappingBot->runMainLoop();
    else
        printf("Invalid state! %d\n", this->mappingBot->state);
}

void CommandHandler::handle(StopMainLoopCommandPacket*)
{
    if(this->mappingBot->state == RUNNING)
        this->mappingBot->stopMainLoop();
    else
        printf("Invalid state! %d\n", this->mappingBot->state);
}