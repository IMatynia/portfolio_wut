#ifndef BLOCK_ITERATOR_HPP
#define BLOCK_ITERATOR_HPP

#include "block.hpp"
#include "sem_set.hpp"
#include <cassert>
#include <cstdint>

namespace ts {

class BlockIterator {
public:
    BlockIterator(Block initial, SemSet &set, uint32_t buckets,
                  uint32_t bucket_size)
        : at(initial), set(set), buckets(buckets), bucket_size(bucket_size),
          past_end(false) {}

    bool init(const timespec *ts = nullptr) {
        set.add(0, SemSet::Op::Lock);
        if (!set.execute(ts))
            return false;
        at = Block(at.memory, 0);
        at.read();
        return true;
    }

    operator bool() const { return !past_end; }

    Block &operator*() { return at; }

    Block *operator->() { return &at; }

    const Block &operator*() const { return at; }

    const Block *operator->() const { return &at; }

    bool has_next() const { return at.header.next != 0; }

    uint32_t offset() const { return at.offset; }

    void drop() {
        if (past_end)
            return;
        set.add(bucket_index(at.offset), SemSet::Op::Unlock);
        set.execute(nullptr);
        past_end = true;
    }

    bool rewind(const timespec *ts = nullptr) {
        drop();
        past_end = false;
        return init(ts);
    }

    ~BlockIterator() { drop(); }

    bool next(const struct timespec *ts = nullptr) {
        if (past_end)
            return true;
        auto current_index = bucket_index(at.offset);
        if (at.header.is_last_in_bucket() && current_index == buckets - 1) {
            drop();
            return true;
        }
        if (at.header.is_last_in_bucket()) {
            set.add(current_index, SemSet::Op::Unlock);
            set.add(current_index + 1, SemSet::Op::Lock);
            if (!set.execute(ts))
                return false;
            at.offset = bucket_size * (current_index + 1);
            at.read();
        } else {
            at.offset = at.header.next;
            at.read();
        }
        return true;
    }

    void operator++() { next(nullptr); }

    void alloc(uint32_t data_size) {
        assert(at.header.is_free());
        uint32_t total_size = sizeof(BlockHeader) + data_size;
        assert(at.header.size >= total_size);

        auto left = at.header.size - total_size;
        if (left <= sizeof(BlockHeader)) {
            at.header.data_size = data_size;
            at.write();
            return;
        }

        BlockHeader alloc_header{
            .prev = at.header.prev,
            .next = at.offset + total_size,
            .size = total_size,
            .data_size = data_size,
        };
        BlockHeader free_header{
            .prev = at.offset,
            .next = at.header.next,
            .size = left,
            .data_size = 0,
        };
        Block alloc(at.memory, at.offset, alloc_header);
        Block free(at.memory, alloc_header.next, free_header);

        if (free.header.has_next_in_bucket()) {
            Block next(at.memory, free_header.next);
            next.header.prev = free.offset;
            next.write();
        }

        at = alloc;
    }

    void free() {
        assert(!at.is_free());
        bool has_prev = at.header.has_prev_in_bucket();
        bool has_next = at.header.has_next_in_bucket();
        bool prev_free = false;
        bool next_free = false;
        if (has_prev) {
            Block prev(at.memory, at.header.prev);
            prev_free = prev.header.is_free();
        }
        if (has_next) {
            Block next(at.memory, at.header.next);
            next_free = next.header.is_free();
        }
        if (!next_free && !prev_free) {
            at.header.data_size = 0;
            at.write();
        } else if (next_free && !prev_free) {
            Block next(at.memory, at.header.next);
            if (next.header.has_next_in_bucket()) {
                Block next_next(at.memory, next.header.next);
                next_next.header.prev = at.offset;
                next_next.write();
            }
            at.header.data_size = 0;
            at.header.size += next.header.size;
            at.header.next = next.header.next;
            at.write();
        } else if (!next_free && prev_free) {
            Block prev(at.memory, at.header.prev);
            prev.header.size += at.header.size;
            prev.header.next = at.header.next;
            if (has_next) {
                Block next(at.memory, at.header.next);
                next.header.prev = prev.offset;
                next.write();
            }
            prev.write();
            at = prev;
        } else {
            Block prev(at.memory, at.header.prev);
            Block next(at.memory, at.header.next);
            if (next.header.has_next_in_bucket()) {
                Block next_next(at.memory, next.header.next);
                next_next.header.prev = prev.offset;
                next_next.write();
            }
            prev.header.size += at.header.size + next.header.size;
            prev.header.next = next.header.next;
            prev.write();
            at = prev;
        }
    }

private:
    unsigned int bucket_index(uint32_t offset) const {
        return offset / bucket_size;
    }

    Block at;
    SemSet &set;
    uint32_t buckets;
    uint32_t bucket_size;
    bool past_end;
};

} // namespace ts

#endif
