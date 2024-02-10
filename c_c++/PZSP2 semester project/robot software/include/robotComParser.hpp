//
// Created by imat on 22.11.23.
//

#ifndef ROBOT_MAPUJACY_ROBOTCOMPARSER_H
#define ROBOT_MAPUJACY_ROBOTCOMPARSER_H

#include "robotComProtol.hpp"
#include "utility.hpp"
#include <string>

class RobotComProtocolParser {
public:
    static RobotComPacket* parsePacketString(const std::string& buffer);
    static std::string serializePacket(MapperDataPointPacket* packet);
};

#endif //ROBOT_MAPUJACY_ROBOTCOMPARSER_H
