#ifndef BLOCK_HEADER_HPP
#define BLOCK_HEADER_HPP

#include <cstddef>
#include <cstdint>
#include <cstring>

namespace ts {

struct BlockHeader {
    uint32_t prev;
    uint32_t next;
    uint32_t size;
    uint32_t data_size;

    void read_from(const char *memory) {
        size_t offset = 0;
        std::memcpy(&prev, memory + offset, sizeof(prev));
        offset += sizeof(prev);
        std::memcpy(&next, memory + offset, sizeof(next));
        offset += sizeof(next);
        std::memcpy(&size, memory + offset, sizeof(size));
        offset += sizeof(size);
        std::memcpy(&data_size, memory + offset, sizeof(data_size));
        offset += sizeof(data_size);
    }

    void write_to(char *memory) const {
        size_t offset = 0;
        std::memcpy(memory + offset, &prev, sizeof(prev));
        offset += sizeof(prev);
        std::memcpy(memory + offset, &next, sizeof(next));
        offset += sizeof(next);
        std::memcpy(memory + offset, &size, sizeof(size));
        offset += sizeof(size);
        std::memcpy(memory + offset, &data_size, sizeof(data_size));
        offset += sizeof(data_size);
    }

    bool is_first_in_bucket() const { return prev == UINT32_MAX; }

    bool has_prev_in_bucket() const { return !is_first_in_bucket(); }

    bool is_last_in_bucket() const { return next == UINT32_MAX; }

    bool has_next_in_bucket() const { return !is_last_in_bucket(); }

    bool is_free() const { return data_size == 0; }
};

} // namespace ts

#endif
