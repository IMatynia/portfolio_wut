#ifndef REAL_TUPLESPACE_H
#define REAL_TUPLESPACE_H

#include "AbstractTuplespaceAPI.h"
#include "RequestQueueSharedMemoryMap.h"
#include "Tuple.h"
#include "Utils.h"
#include "new_tuple_queue.hpp"
#include "tuple_list.hpp"
#include <bits/types/struct_timespec.h>
#include <cstring>
#include <memory>
#include <string>

class RealTuplespace : public AbstractTuplespaceApi {
public:
    RealTuplespace(const std::string &demon_path, key_t notifyingSemKey, int buckets, int buckets_size)
        : demon_path(demon_path), notifyingSemKey(notifyingSemKey), buckets(buckets), buckets_size(buckets_size) {
        if (notifyingSemKey == IPC_PRIVATE) {
            throw std::invalid_argument("Notifying semaphore key cant be set to zero!");
        }
        SPDLOG_INFO("Notifying semaphore key {}", notifyingSemKey);
    }

    void init() override;
    ;

    void TSoutput(const Tuple &newTuple) override;
    ;

    Tuple *TSinput(const TuplePattern &tuplePattern, int timeoutMs) override;
    ;

    Tuple *TSread(const TuplePattern &tuplePattern, int timeoutMs) override;
    ;

    void close() override { destroyNotifyingSemaphore(); };

    ~RealTuplespace() override { close(); }

private:
    Tuple *get_helper(const TuplePattern &tuplePattern, int timeoutMs,
                      bool remove);

    void initNotifyingSemaphore();

    void destroyNotifyingSemaphore();

    void unlockOnSemAwaitingNewTuple();

    bool lockOnSemAwaitingNewTuple(int timeoutMs);

    std::string demon_path;
    int notifyingSemId = 0;
    key_t notifyingSemKey;
    int buckets;
    int buckets_size;
    std::unique_ptr<ts::TupleList> tuple_list;
    std::unique_ptr<ts::NewTupleQueue> new_tuple_queue;
    std::unique_ptr<RequestQueueSharedMemoryMap> requestQueueSharedMemoryMap;

    RequestQueue *requestQueue;
};

#endif
