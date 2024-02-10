/*
 * Development during 23Z semester by
 * I. Matynia
 */

#include "include/IODevice.hpp"

AsyncIODevice::~AsyncIODevice() {
    this->removeCallbacks();
}

void AsyncIODevice::addDataReceivedCallback(IODataCallback_ptr callback) {
    this->dataReceivedCallbacks.insert(callback);
}

void AsyncIODevice::removeDataReceivedCallback(IODataCallback_ptr callback) {
    this->dataReceivedCallbacks.erase(callback);
}

void AsyncIODevice::removeCallbacks() {
    this->dataReceivedCallbacks.clear();
}

void AsyncIODevice::passBufferToCallbacks(const std::string& buffer) {
    for (auto callback: this->dataReceivedCallbacks) {
        (*callback)(buffer);
    }
}

void AsyncIODevice::startCallbackNotifierThread() {
    this->callbackNotifierRunning = true;
    this->callbackNotifierThread = new riot::thread(callbackNotifierThreadTask, this);
}

void AsyncIODevice::stopCallbackNotifierThread() {
    this->callbackNotifierRunning = false;
    msg_t closing_message;
    closing_message.content.ptr = nullptr;
    msg_send(&closing_message, this->callbackNotifierThread->native_handle());
    this->callbackNotifierThread->join();
    delete this->callbackNotifierThread;
}

void callbackNotifierThreadTask(void* asyncIODevice_ptr)
{
    AsyncIODevice* asyncIODevice = (AsyncIODevice*)asyncIODevice_ptr;
    msg_t msg;
    msg_t msg_queue[16];
    msg_init_queue(msg_queue, 8);
    while(asyncIODevice->callbackNotifierRunning)
    {
        msg_receive(&msg);
        char c = (char)msg.content.value;
        if (c == '\0')
        {
            break;
        }
        else
        {
            asyncIODevice->handleRawData(c);
        }
    }
}