#include "RealTuplespace.h"
#include <spdlog/spdlog.h>
#include <sys/time.h>

void RealTuplespace::init() {
    key_t tuple_list_key = ftok(demon_path.c_str(), 0);
    key_t new_tuple_queue_key = ftok(demon_path.c_str(), 1);
    key_t request_queue_key = ftok(demon_path.c_str(), 2);
    tuple_list = std::make_unique<ts::TupleList>(
            tuple_list_key, "/tuple-space-list", false, false, buckets, buckets_size);
    new_tuple_queue = std::make_unique<ts::NewTupleQueue>(
            new_tuple_queue_key, "/tuple-space-new-tuple-queue", false, false,
            1024);
    requestQueueSharedMemoryMap = std::make_unique<RequestQueueSharedMemoryMap>(
            request_queue_key, "/tuple-space-request-queue", false);
    requestQueue = requestQueueSharedMemoryMap->getRequestQueue();
    initNotifyingSemaphore();

    SPDLOG_INFO("Initialized tuplespace api, pid: {}", getpid());
}

void RealTuplespace::TSoutput(const Tuple &newTuple) {
    std::string buffer;
    newTuple.serialize(buffer);
    auto size = buffer.size() + 1;
    auto it = tuple_list->iter();
    it.init();
    for (; it; ++it) {
        if (it->is_used())
            continue;
        if (it->max_data_size() < size)
            continue;
        it.alloc(size);
        std::memcpy(it->data(), buffer.c_str(), size);
        auto offset = it.offset();
        it.drop();
        new_tuple_queue->add(offset);
        return;
    }
}

Tuple *RealTuplespace::get_helper(const TuplePattern &tuplePattern,
                                  int timeoutMs, bool remove) {
    SPDLOG_INFO("Initial lookup of tuples");
    requestQueue->lock();
    auto it = tuple_list->iter();
    it.init();
    for (; it; ++it) {
        if (it->is_free())
            continue;
        std::string buffer;
        buffer.assign(it->data(), it->data_size());
        Tuple t = Tuple::deserialize(buffer);
        if (!t.isReadByWaitingProcesses)
            continue;
        if (tuplePattern.compareSlow(t)) {
            SPDLOG_INFO("Tuple match!");
            if (remove)
                it.free();
            requestQueue->unlock();
            return new Tuple(t);
        }
    }
    it.drop();
    SPDLOG_INFO("Initial lookup failed");
    int remainingTimeout = timeoutMs;
    bool timedout;

    int currentRequestIndex = requestQueue->addRequest({
                                                               .notifyingSemaphoreKey = this->notifyingSemKey,
                                                               .processPid = getpid(),
                                                               .fresh = true,
                                                       });
    SPDLOG_INFO("Added self to the request queue at {}", currentRequestIndex);
    requestQueue->unlock();

    if (currentRequestIndex == INDEXNULL) {
        throw std::runtime_error("Request queue is full!");
    }

    struct timeval then{}, now{};
    gettimeofday(&then, nullptr);

    while (remainingTimeout > 0) {
        timedout = !lockOnSemAwaitingNewTuple(remainingTimeout);
        if (timedout) {
            SPDLOG_INFO("Timed out!");
            requestQueue->lock();
            requestQueue->removeRequest(currentRequestIndex);
            requestQueue->unlock();
            break;
        }

        requestQueue->getElement(currentRequestIndex)->request.fresh =
                false; // freshness - demon shoulndt remove ;fresh; elements from
        // the queue, because noone was sleeping on them yet
        SPDLOG_INFO("New tuple has arrived!");
        // already locked in demon
        uint32_t offset = new_tuple_queue->peek_when_locked();
        auto it = tuple_list->iter();
        it.init();
        for (; it; ++it) {
            if (it.offset() != offset)
                continue;
            std::string buffer;
            buffer.assign(it->data(), it->data_size());
            Tuple t = Tuple::deserialize(buffer);
            SPDLOG_INFO("Got the tuple");
            if (tuplePattern.compareSlow(t)) {
                SPDLOG_INFO("Tuple match!");
                if (remove) {
                    it.free();
                    new_tuple_queue->take_when_locked();
                    SPDLOG_INFO("Tuple removed!");
                }
                it.drop();
                // demon will unlock queue
                unlockOnSemAwaitingNewTuple();
                return new Tuple(t);
            }
            SPDLOG_INFO("Tuple doesnt match!");
        }
        SPDLOG_INFO("No tuples matched.");
        it.drop();
        unlockOnSemAwaitingNewTuple();

        gettimeofday(&now, nullptr);
        long long dt = (now.tv_sec - then.tv_sec) * 1000LL + (now.tv_usec - then.tv_usec) / 1000LL;
        remainingTimeout -= (int)dt;
        then = now;

        SPDLOG_INFO("Remaining timeout: {}", remainingTimeout);
    }
    return nullptr;
}

Tuple *RealTuplespace::TSinput(const TuplePattern &tuplePattern,
                               int timeoutMs) {
    return get_helper(tuplePattern, timeoutMs, true);
}

Tuple *RealTuplespace::TSread(const TuplePattern &tuplePattern, int timeoutMs) {
    return get_helper(tuplePattern, timeoutMs, false);
}

void RealTuplespace::initNotifyingSemaphore() {
    notifyingSemId =
            semget(this->notifyingSemKey, 2, IPC_CREAT | IPC_EXCL | 0666);
    if (notifyingSemId == -1) {
        throw std::system_error(errno, std::generic_category(), "Coulding init notifying semaphore");
    }

    // Set the initial value of the semaphore
    union semun {
        int val;               /* Value for SETVAL */
        struct semid_ds *buf;  /* Buffer for IPC_STAT, IPC_SET */
        unsigned short *array; /* Array for GETALL, SETALL */
        struct seminfo *__buf; /* Buffer for IPC_INFO
                                   (Linux-specific) */
    } a;
    a.val = 0;
    if (semctl(this->notifyingSemId, 0, SETVAL, a) == -1) {
        throw std::system_error(errno, std::generic_category(), "Failed to set initial value");
    }

    a.val = 0;
    if (semctl(this->notifyingSemId, 1, SETVAL, a) == -1) {
        throw std::system_error(errno, std::generic_category(), "Failed to set the initial value of the semaphore.");
    }
}

void RealTuplespace::destroyNotifyingSemaphore() {
    ts::semun dummy{};
    semctl(notifyingSemId, 0, IPC_RMID, dummy);
}

void RealTuplespace::unlockOnSemAwaitingNewTuple() {
    struct sembuf semOps[] = {
            {1, +1, 0},
    };
    if (semop(notifyingSemId, semOps, 1) == -1) {
        throw std::system_error(errno, std::generic_category(), "Failed to unlock demon");
    }
}

bool RealTuplespace::lockOnSemAwaitingNewTuple(int timeoutMs) {
    struct sembuf semOps[] = {
            {0, -1, 0},
    };

    struct timespec timeout = ms_to_timespec(timeoutMs);

    if (semtimedop(notifyingSemId, semOps, 1, &timeout) == -1) {
        if (errno == EAGAIN) {
            return false;
        } else {
            throw std::system_error(errno, std::generic_category(), "Failed to lock sem");

        }
    }

    return true;
}

long long timespec_to_ms(const timespec &ts) {
    return 1000 * ts.tv_sec + ts.tv_nsec / 1'000'000;
}
