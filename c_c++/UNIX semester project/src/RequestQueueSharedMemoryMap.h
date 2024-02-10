//
// Created by imat on 14.01.24.
//

#ifndef TUPLESPACE_REQUESTQUEUESHAREDMEMORYMAP_H
#define TUPLESPACE_REQUESTQUEUESHAREDMEMORYMAP_H

#include "RequestQueue.h"

#include <fcntl.h>
#include <stdexcept>
#include <sys/mman.h>
#include <system_error>
#include <unistd.h>

class RequestQueueSharedMemoryMap {
public:
    RequestQueueSharedMemoryMap(key_t sem_key, const std::string &name,
                                bool create);

    RequestQueue *getRequestQueue();

    ~RequestQueueSharedMemoryMap();

private:
    void initialize();

    bool initializedInThisScope = false;
    char *memory;
    int shmem_fd;
    std::string name;
    key_t sem_key;
};

#endif // TUPLESPACE_REQUESTQUEUESHAREDMEMORYMAP_H
