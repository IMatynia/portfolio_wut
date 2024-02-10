/*
 * Code developed during the PZSP2 course during semester 23L by
 * M. Dabrawski, M. Lojek, M. Kindeusz, R. Ishchuk
 *
 */

#include "include/wallFollower.hpp"
#include <thread.h>
#include <xtimer.h>

#define P_GAIN 1.1
#define D_GAIN 0.3

void* wallFollowThread(void* wallFollowerInstance)
{
	WallFollower* follower = (WallFollower*) wallFollowerInstance;
    printf("Wall follower - seek wall\n");
	follower->dmc->forward(500, 0);
	while(follower->measureMiddle(1) > 250)
	{
		xtimer_msleep(100);
		if(!follower->running)
		{
			follower->dmc->stop();
			return nullptr;
		}
	}

    printf("Wall follower - wall found\n");
	follower->dmc->rotateL(80, 500);

	long prevErrorSignal = 0;
	long direction = 0;

    printf("Wall follower - running\n");
	while(follower->running)
	{
		xtimer_msleep(10);
		if(follower->measureMiddle(1) < 250)
		{
			follower->dmc->rotateL(80, 400);
		}

		//initial value from PZSP2 project 250
		long errorSignal = follower->measureRight(1) - 200;

		direction = (errorSignal * P_GAIN) + ((errorSignal - prevErrorSignal) * D_GAIN);

		if(direction > 250) direction = 250;
		if(direction < -250) direction = -250;

		follower->dmc->forward(400, direction);
	}

	follower->dmc->stop();
    printf("Wall follower - stopping\n");
	return nullptr;
}

WallFollower::WallFollower(DualMotorController* dmc, UltrasonicSensorDriver* sensorM, UltrasonicSensorDriver* sensorR)
{
	this->dmc = dmc;
	this->sensorR = sensorR;
	this->sensorM = sensorM;
	this->followingThreadStack = new char[THREAD_STACKSIZE_MAIN];
	this->running = false;
	this->distanceM = 0;
	this->distanceR = 0;
}

void WallFollower::start()
{
	this->running = true;
	thread_create(this->followingThreadStack, THREAD_STACKSIZE_MAIN,
				  THREAD_PRIORITY_MAIN - 2, THREAD_CREATE_STACKTEST,
				  wallFollowThread, this, "wallFollowingThread");
}

void WallFollower::stop()
{
	this->running = false;
}

WallFollower::~WallFollower()
{
	delete [] this->followingThreadStack;
}

unsigned long WallFollower::getDistanceMiddle() {
	riot::lock_guard<riot::mutex> guard(mtx);
	return distanceM;
}

unsigned long WallFollower::getDistanceRight() {
	riot::lock_guard<riot::mutex> guard(mtx);
	return distanceR;
}

unsigned long WallFollower::measureMiddle(int measurements) {
	riot::lock_guard<riot::mutex> guard(mtx);
	distanceM = 0;
	for (int i = 0; i < measurements; ++i)
		distanceM += sensorM->measure();
	distanceM /= measurements;
	return distanceM;
}

unsigned long WallFollower::measureRight(int measurements) {
	riot::lock_guard<riot::mutex> guard(mtx);
	distanceR = 0;
	for (int i = 0; i < measurements; ++i)
		distanceR += sensorR->measure();
	distanceR /= measurements;
	return distanceR;
}
