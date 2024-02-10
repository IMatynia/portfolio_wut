/*
 * Code developed during the PZSP2 course during semester 23L by
 * M. Dabrawski, M. Lojek, M. Kindeusz, R. Ishchuk
 *
 */

#ifndef INCLUDE_WALLFOLLOWER_HPP_
#define INCLUDE_WALLFOLLOWER_HPP_

#include "dualMotorController.hpp"
#include "ultrasonicSensorDriver.hpp"
#include <riot/mutex.hpp>

class WallFollower
{
public:
	WallFollower(DualMotorController* dmc, UltrasonicSensorDriver* sensorM, UltrasonicSensorDriver* sensorR);
	void start();
	void stop();
	~WallFollower();
	unsigned long getDistanceMiddle();
	unsigned long getDistanceRight();
	unsigned long measureMiddle(int measurements = 1);
	unsigned long measureRight(int measurements = 1);

private:
	DualMotorController* dmc;
	UltrasonicSensorDriver* sensorM;
	UltrasonicSensorDriver* sensorR;
	bool running;
	unsigned long distanceM;
	unsigned long distanceR;
	riot::mutex mtx;
	char* followingThreadStack;
	friend void* wallFollowThread(void* wallFollowerInstance);
};


#endif /* INCLUDE_WALLFOLLOWER_HPP_ */
