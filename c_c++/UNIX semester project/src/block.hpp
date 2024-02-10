#ifndef BLOCK_HPP
#define BLOCK_HPP

#include "block_header.hpp"
#include <fmt/core.h>

namespace ts {

class Block {
public:
    Block(char *memory, uint32_t offset, const BlockHeader &header)
        : memory(memory), offset(offset), header(header) {
        write();
    }

    Block(char *memory, uint32_t offset) : memory(memory), offset(offset) {
        read();
    }

    char *data() { return memory + offset + sizeof(BlockHeader); }

    const char *data() const { return memory + offset + sizeof(BlockHeader); }

    uint32_t data_size() const { return header.data_size; }

    uint32_t size() const { return header.size; }

    uint32_t max_data_size() const { return header.size - sizeof(BlockHeader); }

    void write() { header.write_to(memory + offset); }

    void read() { header.read_from(memory + offset); }

    bool is_free() const { return header.is_free(); }

    bool is_used() const { return !header.is_free(); }

    std::string to_string() const {
        return fmt::format(
            "Block offset: {:<5} prev: {:<5} next: {:<5} size:{:<5} {}", offset,
            header.is_first_in_bucket() ? "bucket start"
                                        : std::to_string(header.prev),
            header.is_last_in_bucket() ? "bucket end"
                                       : std::to_string(header.next),
            header.size,
            is_free() ? "free" : fmt::format("used {}", header.data_size));
    }

    friend class BlockIterator;
    friend class TupleList;

private:
    char *memory;
    uint32_t offset;
    BlockHeader header;
};

} // namespace ts

#endif
