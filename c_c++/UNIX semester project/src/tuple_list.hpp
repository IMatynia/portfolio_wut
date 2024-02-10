#ifndef TUPLE_LIST_HPP
#define TUPLE_LIST_HPP

#include "block.hpp"
#include "block_iterator.hpp"
#include "sem_set.hpp"
#include <fcntl.h>
#include <string>
#include <sys/mman.h>
#include <sys/stat.h>
#include <system_error>
#include <unistd.h>

namespace ts {

class TupleList {
public:
    TupleList(key_t sem_key, const std::string &name, bool create, bool destroy,
              uint32_t buckets, uint32_t bucket_size)
        : name(name), destroy(destroy), buckets(buckets),
          bucket_size(bucket_size), sem_set(sem_key, buckets, create, destroy) {
        int flags = O_RDWR;
        if (create)
            flags |= O_CREAT | O_EXCL;
        int ret = shm_open(name.c_str(), flags, 0666);
        if (ret == -1)
            throw std::system_error(errno, std::generic_category(), "tuple list shm_open");
        fd = ret;
        if (create)
            if (ftruncate(fd, get_size()) == -1)
                throw std::system_error(errno, std::generic_category(),
                                        "tuple list ftruncate");

        mem = static_cast<char *>(mmap(
            nullptr, get_size(), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0));
        if (mem == MAP_FAILED)
            throw std::system_error(errno, std::generic_category(), "tuple list mmap");

        if (create) {
            for (uint32_t i = 0; i < buckets; ++i) {
                BlockHeader header{
                    .prev = UINT32_MAX,
                    .next = UINT32_MAX,
                    .size = bucket_size,
                    .data_size = 0,
                };
                Block(mem, i * bucket_size, header);
            }
        }
    }

    BlockIterator iter() {
        Block start(mem, 0);
        return BlockIterator(start, sem_set, buckets, bucket_size);
    }

    uint32_t get_size() const { return buckets * bucket_size; }

    uint32_t get_bucket_size() const { return bucket_size; }

    uint32_t get_bucket_count() const { return buckets; }

    ~TupleList() {
        munmap(mem, get_size());
        if (destroy)
            shm_unlink(name.c_str());
    }

private:
    std::string name;
    bool destroy;
    int fd;
    char *mem;
    uint32_t buckets;
    uint32_t bucket_size;
    SemSet sem_set;
};

} // namespace ts
#endif
