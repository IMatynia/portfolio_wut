/*
 * Code developed during the PZSP2 course during semester 23L by
 * M. Dabrawski, M. Lojek, M. Kindeusz, R. Ishchuk
 *
 * Addition of button star feature by K. Cabaj
 *
 * Development during 23Z semester by
 * I. Matynia
 */
#include <stdio.h>
#include <shell.h>
#include <string.h>
#include <stdlib.h>
#include <periph/pwm.h>
#include <xtimer.h>
#include <math.h>
#include <string.h>

#include "include/robotContext.hpp"
#include "tests/all.hpp"

void *__dso_handle __attribute__((weak)) = NULL;

static int ping_command(int, char**)
{
	printf("Pong!\n\r");

	return 0;
}

static int now_command(int argc, char**)
{
	unsigned long time = 0;
	if(argc > 1) goto now_usage;

	time = xtimer_now();
	printf("Curent timestamp: %lu\r\n\r\n", time);

	return 0;

	now_usage:
	printf("Usage: now\r\n\r\n");
	return -1;
}

void test_bluetooth_receive_callback(const std::string& buffer)
{
    printf("Packet: %s\n", buffer.c_str());
}

static int test_bluetooth_command(int, char**)
{
    BluetoothIO bt(devicesConfig::bluetooth.device, devicesConfig::bluetooth.baud);
    bt.open();

    IODataCallback_t callback = IODataCallback_t(test_bluetooth_receive_callback);
    bt.addDataReceivedCallback(&callback);
    xtimer_sleep(200000);
    bt.close();
    return 0;
}

static int test_mapping_bot_state_machine(int, char**)
{
    init_hardware();
    init_high_level_divers();
    compose_robot();
    bot->start();
    return 0;
}

static int magnetometer_command(int argc, char**)
{
	(void)argc;
	static double start_angle = 0;
	if (!magnetometer) {
		magnetometer = new Magnetometer();
		auto id = magnetometer->readProductID();
		printf("Product ID: %d\n", id);
		magnetometer->temperatureMeasure();
		double temp = magnetometer->getTemperature();
		printf("Temperature: %f C\n", temp);
		magnetometer->accurateMeasure();
		auto a = magnetometer->getXYZ();
		start_angle = atan2(a[1], a[0]) * 180.0 / M_PI;
	}
	magnetometer->accurateMeasure();
	auto a = magnetometer->getXYZ();
	double angle = atan2(a[1], a[0]) * 180.0 / M_PI;
	printf("X: %ld mG Y: %ld mG Z: %ld mG\n", floatingpoint_to_long(a[0], PRECISION_3), floatingpoint_to_long(a[1], PRECISION_3), floatingpoint_to_long(a[2], PRECISION_3));
	printf("angle: %ld deg\n", floatingpoint_to_long(angle, PRECISION_3));
	double diff = angle - start_angle;
	while (diff > 180)
		diff -= 360;
	while (diff <= -180)
		diff += 360;
	printf("delta: %ld deg\n", floatingpoint_to_long(diff, PRECISION_3));

	return 0;
}

static int calibrate_magnetometer_command(int argc, char**) {
	(void)argc;
	if (!magnetometer) {
		magnetometer = new Magnetometer();
	}
	while (true) {
		magnetometer->accurateMeasure();
		auto a = magnetometer->getRawXYZ();
		printf("%ld,%ld,%ld\n", a[0], a[1], a[2]);
	}
	return 0;
}

static int calibrate_magnetometer_with_motor_command(int argc, char **argv) {
	(void)argc;
	motorR = new MotorDriver(&(devicesConfig::motorRight));
	motorL = new MotorDriver(&(devicesConfig::motorLeft));
	motController = new DualMotorController(motorR, motorL, pulserL, pulserR, 20);
	magnetometer = new Magnetometer();
	xtimer_sleep(1);
	if (argc != 2)
		motController->forward(500, 0);
	else if (strcmp(argv[1], "left"))
		motController->forward(500, -500);
	else if (strcmp(argv[1], "right"))
		motController->forward(500, 500);
	while (true) {
		magnetometer->accurateMeasure();
		auto a = magnetometer->getRawXYZ();
		printf("%ld,%ld,%ld\n", a[0], a[1], a[2]);
	}
	return 0;
}

static const shell_command_t commands[] =
{
	{"ping", "Returns pong", ping_command},
	{"now", "Show current timestamp", now_command},
	{"bt", "Test ble", test_bluetooth_command},
	{"sm", "Test state machine", test_mapping_bot_state_machine},
	{"mag", "Magnetometer", magnetometer_command},
	{"cal", "calibrate magnetometer", calibrate_magnetometer_command},
	{"calmot", "calibrate magnetometer with motor", calibrate_magnetometer_with_motor_command},
	{"test", "run all tests", runAll},
	{NULL, NULL, NULL}
};

void led_callback(void *arg) {
	(void)arg;
	static bool ledOn = false;
	ledOn = !ledOn;
	if (ledOn)
		gpio_set(LED0_PIN);
	else
		gpio_clear(LED0_PIN);
}

int main(void)
{
    initial_button_setup();

    char lineBuf[SHELL_DEFAULT_BUFSIZE];
	shell_run(commands, lineBuf, SHELL_DEFAULT_BUFSIZE);
}
