//
// Created by imat on 14.01.24.
//

#include <system_error>
#include "RequestQueue.h"

RequestQueue::RequestQueue(key_t accessSemaphoreKey, bool deleteSem) {
    emptySpaceIterator = 0;
    first = INDEXNULL;
    last = INDEXNULL;
    n_elements = 0;
    accessSemId = initAccessSem(accessSemaphoreKey);
    this->deleteSem = deleteSem;
}

int RequestQueue::initAccessSem(key_t accessSemKey) {
    int semid;
    if ((semid = semget(accessSemKey, 1, IPC_CREAT | 0666)) == -1) {
        throw std::system_error(errno, std::generic_category(), "Couldn't init access semaphore for request queue");
    }

    union semun {
        int val;
        struct semid_ds *buf_semid;
        unsigned short *array;
        struct seminfo *buf_seminfo;
    } argument{.val = 1};

    if (semctl(semid, 0, SETVAL, argument) == -1) {
        throw std::system_error(errno, std::generic_category(), "Failed to set value");
    }

    return semid;
}

int RequestQueue::addRequest(Request new_request) {
    RequestQueueElement new_element = {.request = new_request,
                                       .prev = last,
                                       .next = INDEXNULL,
                                       .active = true};
    // find an empty slot
    int spaceFound = findEmptySpot();
    if (spaceFound == INDEXNULL) {
        return INDEXNULL;
    }
    // copy data over to the slot
    *(requestQueueBuf + spaceFound) = new_element;
    // update last and first
    if (last != INDEXNULL)
        (requestQueueBuf + last)->next = spaceFound;
    last = spaceFound;
    if (first == INDEXNULL)
        first = spaceFound;
    // update element count
    n_elements++;
    return spaceFound;
}

int RequestQueue::getFirst() const { return first; }

int RequestQueue::findEmptySpot() {
    if (n_elements == RequestQueue::QUEUE_SIZE)
        return INDEXNULL;

    while ((requestQueueBuf + emptySpaceIterator)->active)
        emptySpaceIterator = (emptySpaceIterator + 1) % QUEUE_SIZE;

    return emptySpaceIterator;
}

int RequestQueue::getNElements() const { return n_elements; }

void RequestQueue::removeRequest(int request_element) {
    if (request_element == INDEXNULL ||
        !(requestQueueBuf + request_element)->active)
        throw std::invalid_argument("Cant remove this element");
    int prev = (requestQueueBuf + request_element)->prev;
    int next = (requestQueueBuf + request_element)->next;

    if (request_element == first)
        first = next;
    if (request_element == last)
        last = prev;

    if (prev != INDEXNULL)
        (requestQueueBuf + prev)->next = next;
    if (next != INDEXNULL)
        (requestQueueBuf + next)->prev = prev;
    (requestQueueBuf + request_element)->active = false;
    n_elements--;
}

void RequestQueue::destroyAccessSem() {
    if (semctl(accessSemId, 0, IPC_RMID) == -1) {
        throw std::system_error(errno, std::generic_category(), "Request queue access sem destruction");
    }
}

RequestQueue::~RequestQueue() {
    // gdy tworzymy obiekt tymczasowy do skopiowania
    // do pamięci nic nie niszczymy
}

void RequestQueue::lock() const {
    struct sembuf semaphoreOperation {
        .sem_num = 0, .sem_op = -1, .sem_flg = SEM_UNDO
    };

    if (semop(accessSemId, &semaphoreOperation, 1) == -1) {
        throw std::system_error(errno, std::generic_category(), "Failed to lock access");

    }
}

void RequestQueue::unlock() const {
    struct sembuf semaphoreOperation {
        .sem_num = 0, .sem_op = 1, .sem_flg = SEM_UNDO
    };

    if (semop(accessSemId, &semaphoreOperation, 1) == -1) {
        throw std::system_error(errno, std::generic_category(), "Sem op failed");
    }
}

RequestQueueElement *RequestQueue::getElement(int element_index) {
    return requestQueueBuf + element_index;
}

int RequestQueue::getLockSemId() const { return accessSemId; }
