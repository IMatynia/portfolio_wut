/*
 * Development during 23Z semester by
 * I. Matynia
 */

#ifndef INCLUDE_ROBOTCOMPROTOCOL_HPP_
#define INCLUDE_ROBOTCOMPROTOCOL_HPP_

#include <string>
#include "abstractPacketHandler.hpp"
#include "robotComProtol.hpp"
#include "utility.hpp"

enum packetType {
    RUN_MAIN_LOOP,
    STOP_MAIN_LOOP,
    DATA_POINT
};

class RobotComPacket {
public:
    RobotComPacket(packetType code): code(code) {};
    virtual ~RobotComPacket() {};
    packetType getCode() {return code;}
   virtual void accept(AbstractPacketHandler* handler) = 0;
private:
    packetType code;
};

class RunMainLoopCommandPacket: public RobotComPacket {
public:
    RunMainLoopCommandPacket(): RobotComPacket(RUN_MAIN_LOOP) {};
    void accept(AbstractPacketHandler* handler);
    ~RunMainLoopCommandPacket() {};
};

class StopMainLoopCommandPacket: public RobotComPacket {
public:
    StopMainLoopCommandPacket(): RobotComPacket(STOP_MAIN_LOOP) {};
    void accept(AbstractPacketHandler* handler);
    ~StopMainLoopCommandPacket() {};
};

class MapperDataPointPacket: public RobotComPacket {
public:
    MapperDataPointPacket(double x_pos, double y_pos, double rotation, unsigned long ultrasonicSensorMeasurement): RobotComPacket(DATA_POINT){
        this->x_pos = floatingpoint_to_long(x_pos, PRECISION_3);
        this->y_pos = floatingpoint_to_long(y_pos, PRECISION_3);
        this->rotation = floatingpoint_to_long(rotation, PRECISION_3);
        this->ultrasonicSensorMeasurement = ultrasonicSensorMeasurement;
    };
    void accept(AbstractPacketHandler* handler);
    long x_pos, y_pos, rotation;
    unsigned long ultrasonicSensorMeasurement;
    ~MapperDataPointPacket() {};
};
#endif