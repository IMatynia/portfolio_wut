/*
 * Development during 23Z semester by
 * I. Matynia
 */

#ifndef INCLUDE_IODEVICE_HPP_
#define INCLUDE_IODEVICE_HPP_

#include <string>
#include <set>
#include <algorithm>
#include <functional>
#include <riot/thread.hpp>

#define BLUETOOTH_WAITING_FOR_DATA_SLEEP_DURATION 50

using IODataCallback_t = std::function<void(const std::string&)>;
using IODataCallback_ptr = std::function<void(const std::string&)>*;

class AsyncIODevice{
public:
    /**
     * Opening of a connection
     */
    virtual void open() = 0;
    /**
     * Closing the connection and joining the callback notifier thread
     */
    virtual void close() = 0;
    /**
     * Adds a callback to the callback list to be notified of data
     * @param callback
     */
    void addDataReceivedCallback(IODataCallback_ptr callback);
    /**
     * Removes a callback from the list
     * @param callback
     */
    void removeDataReceivedCallback(IODataCallback_ptr callback);
    /**
     * Sends contents of the buffer to the device
     * @param buffer
     */
    virtual void send(const std::string& buffer) = 0;
    virtual void handleRawData(char byte) { printf("NOT IMPLEMENTED %c\n", byte); };
    void removeCallbacks();
    virtual ~AsyncIODevice();
protected:
    /**
     * passes the buffer to all callbacks
     * @param buffer
     */
    void passBufferToCallbacks(const std::string& buffer);
    /**
     * initializes callback notifier thread
     */
    void startCallbackNotifierThread();
    /**
     * joins the callback notifier thread
     */
    void stopCallbackNotifierThread();

    bool callbackNotifierRunning = false;
    riot::thread* callbackNotifierThread = nullptr;

    std::set<IODataCallback_ptr> dataReceivedCallbacks;
    friend void callbackNotifierThreadTask(void* asyncIODevice_ptr);
};

void callbackNotifierThreadTask(void* asyncIODevice_ptr);
#endif