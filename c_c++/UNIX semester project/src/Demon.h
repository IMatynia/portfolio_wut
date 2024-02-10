//
// Created by imat on 22.12.23.
//

#ifndef TUPLESPACE_DEMON_H
#define TUPLESPACE_DEMON_H

#include "RequestQueueSharedMemoryMap.h"
#include "new_tuple_queue.hpp"
#include "tuple_list.hpp"
#include <cstddef>
#include <exception>
#include <fcntl.h>
#include <iostream>
#include <memory>
#include <signal.h>
#include <string>
#include <sys/ipc.h>
#include <sys/sem.h>

#define DEMON_MAX_PROCESSING_TIME_MS 500

class Demon {
public:
    explicit Demon(char *path, int buckets, int buckets_size);

    ~Demon();

    void start();

    static void stop();

private:
    void init();

    void mainLoop();

    static void registerSignalHandlers();

    static bool notifyClientAboutATupleAndLetThemProcess(key_t clientSemKey);

    int checkAnyProcessWaitingOnSemaphore(int clientSemId);

    int getSemId(key_t key);

    char *path;

    void initSharedMemory();

    int buckets;
    int buckets_size;

    RequestQueueSharedMemoryMap *requestQueueSharedMemoryMap = nullptr;
    ts::TupleList *tupleList = nullptr;
    ts::NewTupleQueue *newTupleQueue = nullptr;

    RequestQueue *requestQueue = nullptr;
};

#endif // TUPLESPACE_DEMON_H
