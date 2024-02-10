#include "include/robotComParser.hpp"

RobotComPacket* RobotComProtocolParser::parsePacketString(const std::string& buffer) {
    RobotComPacket *out = nullptr;
    if (buffer == "run") {
        out = new RunMainLoopCommandPacket();
    } else if (buffer == "stop") {
        out = new StopMainLoopCommandPacket();
    } else {
        printf("No matcing packet type for %s!\n", buffer.c_str());
    }
    return out;
}

std::string RobotComProtocolParser::serializePacket(MapperDataPointPacket *packet) {
    char *output_buffer = new char[256];
    sprintf(output_buffer,
            "%d,%ld,%ld,%ld,%ld;",
            packet->getCode(),
            packet->x_pos,
            packet->y_pos,
            packet->rotation,
            packet->ultrasonicSensorMeasurement);
    std::string out = std::string(output_buffer);
    delete[] output_buffer;
    return out;
}