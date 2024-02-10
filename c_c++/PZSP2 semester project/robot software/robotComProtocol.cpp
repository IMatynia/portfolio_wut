#include "include/robotComProtol.hpp"

void RunMainLoopCommandPacket::accept(AbstractPacketHandler* handler) {handler->handle(this);}
void StopMainLoopCommandPacket::accept(AbstractPacketHandler* handler) {handler->handle(this);}
void MapperDataPointPacket::accept(AbstractPacketHandler* handler) {handler->handle(this);}
