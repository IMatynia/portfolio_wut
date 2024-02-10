/*
 * Code developed during the PZSP2 course during semester 23L by
 * M. Dabrawski, M. Lojek, M. Kindeusz, R. Ishchuk
 *
 */

#ifndef INCLUDE_POSITIONTRACKER_HPP_
#define INCLUDE_POSITIONTRACKER_HPP_

#include <stdio.h>
#include <riot/mutex.hpp>
#include "pulserDriver.hpp"
#include "magnetometer.hpp"

class PositionTracker
{
public:
	PositionTracker(Magnetometer *magnetometer, PulserDriver* pulserL, PulserDriver* pulserR);
	void start();
	void stop();
	~PositionTracker();

	double getAngle();
	double getX();
	double getY();
	std::array<double, 2> getXY();
private:
	void update();
	double measure();
	PulserDriver* pulserL;
	PulserDriver* pulserR;
	Magnetometer *magnetometer;
	unsigned long previous_left_count;
	unsigned long previous_right_count;
	double position_x;
	double position_y;
	double angle;
	double start_angle;

	char* updatingThreadStack;
	bool running;
	friend void* PositionTracker_updatingThread(void*);

	riot::mutex mtx;
};



#endif /* INCLUDE_POSITIONTRACKER_HPP_ */
