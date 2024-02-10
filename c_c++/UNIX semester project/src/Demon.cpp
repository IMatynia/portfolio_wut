//
// Created by imat on 22.12.23.
//

#include "Demon.h"
#include "Tuple.h"
#include "Utils.h"
#include <spdlog/spdlog.h>


void Demon::registerSignalHandlers() {
    struct sigaction sa{};
    sa.sa_handler = quit_signal_handler;
    sa.sa_flags = 0;
    sigfillset(&sa.sa_mask);
    sigaction(SIGTERM, &sa, nullptr);
    sigaction(SIGINT, &sa, nullptr);
}

void Demon::init() {
    registerSignalHandlers();
    initSharedMemory();
}

void Demon::initSharedMemory() {
    key_t tuple_list_key = ftok(path, 0);
    key_t new_tuple_queue_key = ftok(path, 1);
    key_t request_queue_key = ftok(path, 2);

    // mapping objects onto the shared memory
    tupleList = new ts::TupleList(tuple_list_key, "/tuple-space-list", true,
                                  true, buckets, buckets_size);
    newTupleQueue = new ts::NewTupleQueue(
            new_tuple_queue_key, "/tuple-space-new-tuple-queue", true, true, 1024);
    requestQueueSharedMemoryMap = new RequestQueueSharedMemoryMap(
            request_queue_key, "/tuple-space-request-queue", true);
    requestQueue = requestQueueSharedMemoryMap->getRequestQueue();
}

void Demon::start() {
    init();
    mainLoop();
}

void Demon::stop() { quit = 1; }

void Demon::mainLoop() {
    while (!quit) {
        SPDLOG_INFO("Waiting for new tuples...");
        newTupleQueue->wait_nonempty_and_lock();
        auto new_tuple_size = newTupleQueue->read_size_when_locked();
        auto new_tuple_offset = newTupleQueue->peek_when_locked();
        auto new_tuple_token = newTupleQueue->peek_token_when_locked();
        SPDLOG_INFO(
                "New tuple on queue, queue size: {}, tuple offset {}, token {}",
                new_tuple_size, new_tuple_offset, new_tuple_token);
        requestQueue->lock();
        bool taken = false;
        for (int it = requestQueue->getFirst(); it != INDEXNULL;) {
            // check for dead requests (processes that timed out or died)
            Request request = requestQueue->getElement(it)->request;
            SPDLOG_INFO("Checking request from PID {}", request.processPid);
            int clientSemId = getSemId(request.notifyingSemaphoreKey);

            if (!request.fresh &&
                checkAnyProcessWaitingOnSemaphore(clientSemId)) {
                SPDLOG_INFO("Process timed out, cleanup");
                auto old = it;
                it = requestQueue->getElement(it)->next;
                requestQueue->removeRequest(old);
                continue;
            }
            SPDLOG_INFO("Notifying process {}", request.processPid);
            bool ok = notifyClientAboutATupleAndLetThemProcess(clientSemId);

            SPDLOG_INFO("Process {} finished processing", request.processPid);

            auto current_token = newTupleQueue->peek_token_when_locked();
            auto current_size = newTupleQueue->read_size_when_locked();
            // other took it
            if (current_size == 0 || new_tuple_token != current_token) {
                SPDLOG_INFO("Process {} took the tuple.", request.processPid);
                requestQueue->removeRequest(it);
                taken = true;
                break;
            }
            if (!ok) {
                SPDLOG_INFO("Process {} timed out or crashed, cleanup",
                            request.processPid);
                auto old = it;
                it = requestQueue->getElement(it)->next;
                requestQueue->removeRequest(old);
                continue;
            }
            it = requestQueue->getElement(it)->next;
        }
        SPDLOG_INFO("Finished notifying");
        if (taken) {
            SPDLOG_INFO("The new tuple has been taken!");
            newTupleQueue->unlock_after_wait(true);
            requestQueue->unlock();
            continue;
        }

        newTupleQueue->take_when_locked();
        newTupleQueue->unlock_after_wait(true);
        requestQueue->unlock();

        SPDLOG_INFO("Marking tuple as visited ");

        auto it = tupleList->iter();
        it.init();
        for (; it; ++it) {
            if (it.offset() != new_tuple_offset)
                continue;
            std::string buffer;
            buffer.assign(it->data(), it->data_size());
            Tuple t = Tuple::deserialize(buffer);
            t.isReadByWaitingProcesses = true;
            t.serialize(buffer);
            auto size = buffer.size() + 1;
            std::memcpy(it->data(), buffer.c_str(), size);
            SPDLOG_INFO("Marked as visited");
            break;
        }
        it.drop();
    }
}

Demon::~Demon() {
    requestQueue->destroyAccessSem();
    delete requestQueueSharedMemoryMap;
    delete tupleList;
    delete newTupleQueue;
}

Demon::Demon(char *path, int buckets, int buckets_size) {
    this->path = path;
    this->buckets = buckets;
    this->buckets_size = buckets_size;
}

bool Demon::notifyClientAboutATupleAndLetThemProcess(int clientSemId) {
    struct sembuf semops[] = {
            {0, 1,  0},
            {1, -1, 0},
    };

    // unlock the process
    if (semop(clientSemId, semops + 0, 1) == -1) {
        return false;
    }

    struct timespec timeout = ms_to_timespec(DEMON_MAX_PROCESSING_TIME_MS);

    // wait fo it to finish
    if (semtimedop(clientSemId, semops + 1, 1, &timeout) == -1) {
        if (errno == EAGAIN) {
            return false;
        } else if (errno == EINVAL || errno == EIDRM) {
            // process picked it and already finished
            // destroying the semaphore
            return false;
        } else {
            throw std::system_error(errno, std::generic_category(), "Failed to lock on semaphore.");
        }
    }

    return true;
}

int Demon::checkAnyProcessWaitingOnSemaphore(int clientSemId) {
    if (clientSemId == -1)
        return false;

    struct sembuf checkOp = {0, 0, IPC_NOWAIT};

    if (semop(clientSemId, &checkOp, 1) == -1) {
        if (errno == EAGAIN) {
            return true;
        } else {
            SPDLOG_ERROR("Failed to check the semaphore status, errno {}",
                         errno);
            return false;
        }
    }
    return false;
}

int Demon::getSemId(key_t key) {
    int semId = semget(key, 2, 0);
    return semId;
}

