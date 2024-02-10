/*
 * Code developed during the PZSP2 course during semester 23L by
 * M. Dabrawski, M. Lojek, M. Kindeusz, R. Ishchuk
 * Development during 23Z semester by
 * I. Matynia
 */

#ifndef INCLUDE_BLUETOOTH_HPP_
#define INCLUDE_BLUETOOTH_HPP_

#include "IODevice.hpp"
#include "utility.hpp"

#include <stdio.h>
#include <xtimer.h>
#include <string>
#include <periph/uart.h>
#include <riot/mutex.hpp>

enum statusCode
{
    BT_CONNECTED,
    BT_DISCONNECTED,
    BT_WAITING_FOR_CONNECTION,
};


class BluetoothIO : public AsyncIODevice
{
public:
    BluetoothIO(uart_t device, uint32_t baud, char packetSeparator=';'): uart_device(device), uart_baud(baud), packetSeparator(packetSeparator){};
    ~BluetoothIO();
    void open();
    void close();
    void send(const std::string& buffer); //TODO: concurrency
    void handleRawData(char byte);

    statusCode getBluetoothStatus() { return this->currentStatus; };
private:
    int initUartConnection();
    void setUpBluetooth();
    void forceSend(std::string buffer);
    statusCode currentStatus;

    riot::mutex send_mtx;
    uart_t uart_device;
    uint32_t uart_baud;
    char packetSeparator;
    std::string buffer;

    friend void uart_rx_callback(void *bluetoothIO_ptr, uint8_t character);
};

void uart_rx_callback(void *bluetoothIO_ptr, uint8_t character);

#endif /* INCLUDE_BLUETOOTH_HPP_ */
