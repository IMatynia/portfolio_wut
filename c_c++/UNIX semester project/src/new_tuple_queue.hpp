#ifndef NEW_TUPLE_QUEUE_HPP
#define NEW_TUPLE_QUEUE_HPP

#include "sem_set.hpp"
#include <bits/types/struct_timespec.h>
#include <bits/types/time_t.h>
#include <cstdint>
#include <cstring>
#include <fcntl.h>
#include <stdint.h>
#include <string>
#include <sys/mman.h>
#include <system_error>
#include <unistd.h>

namespace ts {
class NewTupleQueue {
public:
    NewTupleQueue(key_t sem_key, const std::string &name, bool create,
                  bool destroy, uint32_t capacity)
        : name(name), destroy(destroy), sem_set(sem_key, 3, create, destroy) {
        int flags = O_RDWR;
        if (create)
            flags |= O_CREAT | O_EXCL;
        int ret = shm_open(name.c_str(), flags, 0666);
        if (ret == -1)
            throw std::system_error(errno, std::generic_category(), "new tuple queue shm_open");
        fd = ret;
        uint32_t size = sizeof(Item) * capacity + sizeof(Header);
        if (create)
            if (ftruncate(fd, size) == -1)
                throw std::system_error(errno, std::generic_category(),
                                        "new tuple queue ftruncate");

        mem = static_cast<char *>(
            mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0));
        if (mem == MAP_FAILED)
            throw std::system_error(errno, std::generic_category(), "new tuple queue mmap");
        if (create) {
            Header header = {
                .write = 0,
                .read = 0,
                .size = 0,
                .capacity = capacity,
                .next_token = 1,
            };
            std::memcpy(mem, &header, sizeof(header));
            sem_set.set_value(SEM_EMPTY, capacity);
            sem_set.set_value(SEM_FULL, 0);
            sem_set.set_value(SEM_MTX, 1);
            Item *buffer = reinterpret_cast<Item *>(mem + sizeof(Header));
            for (uint32_t i = 0; i < capacity; ++i) {
                buffer[i].value = UINT32_MAX;
                buffer[i].token = 0;
            }
        }
    }

    ~NewTupleQueue() {
        Header header;
        std::memcpy(&header, mem, sizeof(Header));
        uint32_t size = sizeof(Item) * header.capacity + sizeof(Header);
        munmap(mem, size);
        if (destroy)
            shm_unlink(name.c_str());
    }

    bool add(uint32_t value, const timespec *ts = nullptr) {
        sem_set.add(SEM_EMPTY, SemSet::Op::Down);
        sem_set.add(SEM_MTX, SemSet::Op::Lock);
        if (!sem_set.execute(ts))
            return false;
        Header header;
        std::memcpy(&header, mem, sizeof(Header));
        Item *buffer = reinterpret_cast<Item *>(mem + sizeof(Header));
        buffer[header.write].value = value;
        buffer[header.write].token = header.next_token;
        header.next_token += 1;
        header.write = (header.write + 1) % header.capacity;
        header.size += 1;
        std::memcpy(mem, &header, sizeof(Header));
        sem_set.add(SEM_FULL, SemSet::Op::Up);
        sem_set.add(SEM_MTX, SemSet::Op::Unlock);
        sem_set.execute(nullptr);
        return true;
    }

    uint32_t take(const timespec *ts = nullptr) {
        sem_set.add(SEM_FULL, SemSet::Op::Down);
        sem_set.add(SEM_MTX, SemSet::Op::Lock);
        if (!sem_set.execute(ts))
            return false;
        uint32_t value;
        Header header;
        std::memcpy(&header, mem, sizeof(Header));
        Item *buffer = reinterpret_cast<Item *>(mem + sizeof(Header));
        value = buffer[header.read].value;
        header.read = (header.read + 1) % header.capacity;
        header.size -= 1;
        std::memcpy(mem, &header, sizeof(Header));
        sem_set.add(SEM_EMPTY, SemSet::Op::Up);
        sem_set.add(SEM_MTX, SemSet::Op::Unlock);
        sem_set.execute(nullptr);
        return value;
    }

    void wait_nonempty_and_lock(const timespec *ts = nullptr) {
        sem_set.add(SEM_FULL, SemSet::Op::Down);
        sem_set.add(SEM_MTX, SemSet::Op::Lock);
        sem_set.execute(ts);
    }

    uint32_t peek_when_locked() {
        Header header;
        std::memcpy(&header, mem, sizeof(Header));
        Item *buffer = reinterpret_cast<Item *>(mem + sizeof(Header));
        return buffer[header.read].value;
    }

    uint32_t peek_token_when_locked() {
        Header header;
        std::memcpy(&header, mem, sizeof(Header));
        Item *buffer = reinterpret_cast<Item *>(mem + sizeof(Header));
        return buffer[header.read].token;
    }

    void unlock_after_wait(bool taken) {
        if (!taken) {
            sem_set.add(SEM_FULL, SemSet::Op::Up);
            sem_set.add(SEM_MTX, SemSet::Op::Unlock);
            sem_set.execute(nullptr);
        } else {
            sem_set.add(SEM_EMPTY, SemSet::Op::Up);
            sem_set.add(SEM_MTX, SemSet::Op::Unlock);
            sem_set.execute(nullptr);
        }
    }

    uint32_t take_when_locked() {
        Header header;
        std::memcpy(&header, mem, sizeof(Header));
        Item *buffer = reinterpret_cast<Item *>(mem + sizeof(Header));
        uint32_t value = buffer[header.read].value;
        header.read = (header.read + 1) % header.capacity;
        header.size -= 1;
        std::memcpy(mem, &header, sizeof(Header));
        return value;
    }

    uint32_t read_size_when_locked() {
        Header header;
        std::memcpy(&header, mem, sizeof(Header));
        return header.size;
    }

private:
    static const unsigned int SEM_MTX = 0;
    static const unsigned int SEM_EMPTY = 1;
    static const unsigned int SEM_FULL = 2;

    struct Header {
        uint32_t write;
        uint32_t read;
        uint32_t size;
        uint32_t capacity;
        uint32_t next_token;
    };

    struct Item {
        uint32_t value;
        uint32_t token;
    };

    std::string name;
    int fd;
    bool destroy;
    char *mem;
    SemSet sem_set;
};
} // namespace ts

#endif
