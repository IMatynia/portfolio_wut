//
// Created by imat on 24.11.23.
//

#ifndef ROBOT_MAPUJACY_PROTOCOLTESTS_H
#define ROBOT_MAPUJACY_PROTOCOLTESTS_H

#include "../include/robotComParser.hpp"
#include "../include/robotComProtol.hpp"

bool test_protocol_string_parser()
{
    std::string s1 = "run";
    std::string s2 = "stop";
    std::string s3 = "thisonedoesnotexist";
    bool success = true;

    success &= RobotComProtocolParser::parsePacketString(s1)->getCode() == RUN_MAIN_LOOP;
    success &= RobotComProtocolParser::parsePacketString(s2)->getCode() == STOP_MAIN_LOOP;
    success &= RobotComProtocolParser::parsePacketString(s3) == nullptr;

    return success;
}

#endif //ROBOT_MAPUJACY_PROTOCOLTESTS_H
