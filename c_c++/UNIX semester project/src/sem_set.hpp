#ifndef SEM_SET_HPP
#define SEM_SET_HPP

#include "spdlog/spdlog.h"
#include <sys/sem.h>
#include <system_error>
#include <vector>

namespace ts {

namespace {
union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
};
} // namespace

class SemSet {
public:
    enum class Op {
        Unlock,
        Up,
        Lock,
        Down,
        WaitForZero,
    };

    SemSet(key_t key, int nsems, bool create, bool destroy)
        : destroy(destroy), nsems(nsems) {
        int flags = 0666;
        if (create)
            flags |= IPC_CREAT | IPC_EXCL;
        semid = semget(key, nsems, flags);
        if (semid == -1)
            throw std::system_error(errno, std::generic_category(), "semset semget");
        if (create)
            for (int i = 0; i < nsems; ++i) {
                semun arg{.val = 1};
                if (semctl(semid, i, SETVAL, arg) == -1)
                    throw std::system_error(errno, std::generic_category(),
                                            "semset semctl");
            }
    }

    ~SemSet() {
        if (destroy) {
            semun dummy;
            semctl(semid, 0, IPC_RMID, dummy);
        }
    }

    void add(unsigned short sem_num, Op op) {
        struct sembuf buf = {};
        buf.sem_num = sem_num;
        buf.sem_flg = 0;
        switch (op) {
        case Op::Up:
        case Op::Unlock:
            buf.sem_op = 1;
            break;
        case Op::Down:
        case Op::Lock:
            buf.sem_op = -1;
            break;
        case Op::WaitForZero:
            buf.sem_op = 0;
            break;
        }
        ops.push_back(buf);
    }

    void set_value(unsigned short sem_num, unsigned int value) {
        semun dummy;
        dummy.val = value;
        if (semctl(semid, sem_num, SETVAL, dummy) == -1)
            throw std::system_error(errno, std::generic_category(), "semset semctl");
    }

    int sem_count() const { return nsems; }

    void clear() { ops.clear(); }

    bool execute(const struct timespec *ts) {
        if (ops.empty())
            return true;
        spdlog::trace("");
        for (const auto &op : ops) {
            spdlog::trace("semop sem_num: {} sem_op: {} sem_flg: {}",
                          op.sem_num, op.sem_op, op.sem_flg);
        }
        if (semtimedop(semid, ops.data(), ops.size(), ts) == -1) {
            if (errno == EAGAIN) {
                clear();
                return false;
            }
            throw std::system_error(errno, std::generic_category(), "semset semop");
        }
        clear();
        return true;
    }

private:
    bool destroy;
    int nsems;
    int semid;
    std::vector<struct sembuf> ops;
};

} // namespace ts

#endif
