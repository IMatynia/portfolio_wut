//
// Created by imat on 14.01.24.
//

#ifndef TUPLESPACE_REQUESTQUEUE_H
#define TUPLESPACE_REQUESTQUEUE_H

#include <stdexcept>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>

#define INDEXNULL -1

struct Request {
    key_t notifyingSemaphoreKey;
    pid_t processPid;
    int fresh;
};

struct RequestQueueElement {
    Request request;
    int prev;
    int next;
    bool active;
};

class RequestQueue {
public:
    static const int QUEUE_SIZE = 1024;

    explicit RequestQueue(key_t accessSemaphoreKey, bool deleteSem = true);

    ~RequestQueue();

    RequestQueueElement *getElement(int element_index);

    int addRequest(Request new_request);

    void removeRequest(int request_element);

    int getFirst() const;

    int getNElements() const;

    void lock() const;

    void unlock() const;

    int getLockSemId() const;

    RequestQueueElement *getBuf() { return requestQueueBuf; }

    void destroyAccessSem();

private:
    RequestQueueElement requestQueueBuf[RequestQueue::QUEUE_SIZE] = {};
    int first;
    int last;
    int emptySpaceIterator;
    int n_elements;
    int accessSemId;
    bool deleteSem;

    int findEmptySpot();

    int initAccessSem(key_t accessSemKey);
    friend class RequestQueueSharedMemoryMap;
};

#endif // TUPLESPACE_REQUESTQUEUE_H
