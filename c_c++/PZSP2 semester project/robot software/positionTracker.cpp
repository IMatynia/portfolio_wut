/*
 * Code developed during the PZSP2 course during semester 23L by
 * M. Dabrawski, M. Lojek, M. Kindeusz, R. Ishchuk
 *
 */

#include "include/positionTracker.hpp"
#include <stdio.h>
#include <xtimer.h>
#include <math.h>
#include "include/utility.hpp"

// physical parameters of the robot
const static long wheelbase = 135L; // distance between the two wheels.
const static long wheel_circumference = 210L;
const static long encoder_resolution = 40L; // impulses per full revolution of wheel
const static int delay = 100;
const static int magnetometerMeasurements = 8; // how many measurements to calculate the average

static float angle_to_zero_2pi(float angle) {
	angle += 2 * M_PI;
	while (angle < 0) angle += 2 * M_PI;
	while (angle >= 2 * M_PI) angle -= 2 * M_PI;
	return angle;
}

static float angle_to_minus_plus_pi(float angle) {
	angle += M_PI;
	while (angle < 0) angle += 2 * M_PI;
	while (angle >= 2 * M_PI) angle -= 2 * M_PI;
	return angle - M_PI;
}

void* PositionTracker_updatingThread(void* positionTrackerInstance)
{
	printf("Position tracker running\r\n");
	PositionTracker* instance = ((PositionTracker*) positionTrackerInstance);

	while(instance->running)
	{
		instance->update();
//		printf("%f,%f,200\r\n", instance->position_x, instance->position_y);
		xtimer_msleep(delay);
	}

	return nullptr;
}

PositionTracker::PositionTracker(Magnetometer *magnetometer, PulserDriver* pulserL, PulserDriver* pulserR)
{
	this->pulserL = pulserL;
	this->pulserR = pulserR;
	this->magnetometer = magnetometer;
	this->running = false;
	this->previous_left_count = 0;
	this->previous_right_count = 0;
	this->position_x = 0;
	this->position_y = 0;
	this->angle = 0;
	this->updatingThreadStack = new char[THREAD_STACKSIZE_MAIN];
	
	this->start_angle = measure();
}

void PositionTracker::start()
{
	// no need to lock here as this should only be called once
	// and the thread isn't running yet
	this->start_angle = measure();
	this->running = true;
	thread_create(this->updatingThreadStack, THREAD_STACKSIZE_MAIN,
					  THREAD_PRIORITY_MAIN - 2, THREAD_CREATE_STACKTEST,
					  PositionTracker_updatingThread, this, "positionTrackerThread");
    printf("position tracker thread pid: %d\n", thread_getpid());
}


void PositionTracker::stop()
{
	riot::lock_guard<riot::mutex> g(mtx);
	this->running = false;
}


void PositionTracker::update()
{
	auto current_left_count = this->pulserL->read();
	auto current_right_count = this->pulserR->read();

	auto delta_left_count =  current_left_count - this->previous_left_count;
	auto delta_right_count =  current_right_count - this->previous_right_count;

	this->previous_left_count = current_left_count;
	this->previous_right_count = current_right_count;

	// Calculate the distance traveled by each wheel:
	double distance_left_wheel = static_cast<double>(delta_left_count * wheel_circumference) / encoder_resolution;
	double distance_right_wheel =  static_cast<double>(delta_right_count * wheel_circumference) / encoder_resolution;

	// Calculate the average distance traveled by the robot:
	double distance_average = (distance_left_wheel + distance_right_wheel) / 2;

	double new_angle = angle_to_zero_2pi(measure() + M_PI / 2 - start_angle);
	double old_angle = angle_to_zero_2pi(this->angle);
	double avg_angle = angle_to_zero_2pi((old_angle + new_angle) / 2);

	// Update the robot's position and orientation:
	riot::lock_guard<riot::mutex> g(mtx);
	this->position_x += distance_average * cos(avg_angle);
	this->position_y += distance_average * sin(avg_angle);
	this->angle = angle_to_minus_plus_pi(new_angle);
}

PositionTracker::~PositionTracker()
{
	delete [] this->updatingThreadStack;
}

double PositionTracker::getAngle() {
	riot::lock_guard<riot::mutex> g(mtx);
	return angle;
}

double PositionTracker::getX() {
	riot::lock_guard<riot::mutex> g(mtx);
	return position_x;
}

double PositionTracker::getY() {
	riot::lock_guard<riot::mutex> g(mtx);
	return position_y;
}

double PositionTracker::measure() {
	int32_t rx = 0;
	int32_t ry = 0;
	int32_t rz = 0;
	for (int i = 0; i < magnetometerMeasurements; ++i) {
		magnetometer->accurateMeasure();
		auto a = magnetometer->getRawXYZ();
		rx += a[0];
		ry += a[1];
		rz += a[2];
	}
	rx /= magnetometerMeasurements;
	ry /= magnetometerMeasurements;
	rz /= magnetometerMeasurements;
	auto c = magnetometer->getCalibrated({rx, ry, rz});
	// for some reason x has to be negative
	// to give counterclockwise angle
	return std::atan2(c[1], -c[0]);
}

std::array<double, 2> PositionTracker::getXY() {
	riot::lock_guard<riot::mutex> g(mtx);
	return {position_x, position_y};
}
