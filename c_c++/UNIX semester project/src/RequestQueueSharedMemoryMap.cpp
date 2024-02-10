//
// Created by imat on 14.01.24.
//

#include "RequestQueueSharedMemoryMap.h"
#include <system_error>

RequestQueue *RequestQueueSharedMemoryMap::getRequestQueue() {
    return ((RequestQueue *)memory);
}

RequestQueueSharedMemoryMap::RequestQueueSharedMemoryMap(
    key_t sem_key, const std::string &name, bool create) {
    this->sem_key = sem_key;
    this->name = name;
    initializedInThisScope = create;

    // flags
    int flags = O_RDWR;
    if (create)
        flags |= O_CREAT | O_EXCL;

    // opening / creating shared memory
    shmem_fd = shm_open(name.c_str(), flags, 0666);
    if (shmem_fd == -1)
        throw std::system_error(errno, std::generic_category(), "Couldnt open request queue shared memory");

    // truncate file to the correct size, only if create
    long size = sizeof(RequestQueue);
    if (create && ftruncate(shmem_fd, size) == -1)
        throw std::system_error(errno, std::generic_category(), "Coudlnt resize request queue shared memory");

    memory = (char *)mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED,
                          shmem_fd, 0);
    if (memory == MAP_FAILED)
        throw std::system_error(errno, std::generic_category(), "Couldnt map request queue shared memory");

    if (create)
        initialize();
}

void RequestQueueSharedMemoryMap::initialize() {
    *(RequestQueue *)memory = RequestQueue(sem_key, false);
    this->getRequestQueue()->deleteSem = true;
}

RequestQueueSharedMemoryMap::~RequestQueueSharedMemoryMap() {
    munmap(memory, sizeof(RequestQueue));
    if (initializedInThisScope) {
        shm_unlink(name.c_str());
    }
}
