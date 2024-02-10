#include "../src/tuple_list.hpp"
#include <gtest/gtest.h>

using namespace ts;

static constexpr uint32_t TS_BUCKETS = 8;
static constexpr uint32_t TS_ALLOC_SIZE = 8;
static constexpr uint32_t TS_BUCKET_SIZE = 512;
static constexpr uint32_t TS_SIZE = TS_BUCKETS * TS_BUCKET_SIZE;

TupleList make_list() {
    key_t key = 0xDEADBEEF;
    return TupleList(key, "/tuple-space-test", true, true, TS_BUCKETS,
                     TS_BUCKET_SIZE);
}

TEST(TupleSpace, Create) {
    auto ts = make_list();
    auto it = ts.iter();
    it.init();
    EXPECT_TRUE(bool(it));
    for (uint32_t i = 0; i < TS_BUCKETS; ++i) {
        EXPECT_TRUE(it->is_free());
        ++it;
    }
    EXPECT_FALSE(bool(it));
}

TEST(TupleSpace, AllocOnce) {
    auto ts = make_list();
    std::string data = "example data";
    auto data_size = data.size() + 1;
    auto it = ts.iter();
    it.init();
    it.alloc(data_size);
    std::memcpy(it->data(), data.c_str(), data_size);
    it.rewind();

    EXPECT_TRUE(it->is_used());
    EXPECT_EQ(it->data_size(), data_size);
    std::string read = it->data();
    EXPECT_EQ(data, read);
    ++it;
    for (uint32_t i = 0; i < TS_BUCKETS; ++i) {
        EXPECT_TRUE(it->is_free());
        ++it;
    }
    it.rewind();

    it.free();
    for (uint32_t i = 0; i < TS_BUCKETS; ++i) {
        EXPECT_TRUE(it->is_free());
        ++it;
    }
}

TEST(TupleSpace, AllocWhole) {
    auto ts = make_list();
    auto it = ts.iter();
    it.init();
    const auto data_size = it->max_data_size();
    it.alloc(data_size);
    EXPECT_TRUE(it->is_used());
    EXPECT_EQ(it->data_size(), data_size);
    it.free();
    EXPECT_TRUE(it->is_free());
}

TEST(TupleSpace, AllocMany) {
    auto ts = make_list();
    auto it = ts.iter();
    it.init();
    for (int i = 0; i < 4; ++i) {
        it.alloc(sizeof(int));
        ++it;
    }
    it.rewind();

    uint32_t total_size = 0;
    for (int i = 0; i < 4; ++i) {
        EXPECT_EQ(sizeof(int), it->data_size());
        total_size += it->size();
        ++it;
    }
    it.rewind();

    for (int i = 0; i < 4; ++i) {
        it.free();
        ++it;
    }
    it.rewind();

    EXPECT_TRUE(it->is_free());
}

TEST(TupleSpace, FreeNoMerge) {
    constexpr uint32_t alloc_size = 8;

    auto ts = make_list();
    auto it = ts.iter();
    it.init();
    for (int i = 0; i < 3; ++i) {
        it.alloc(alloc_size);
        ++it;
    }
    it.rewind();

    ++it;
    it.free();
    it.rewind();

    EXPECT_TRUE(it->is_used());
    EXPECT_EQ(it->data_size(), alloc_size);
    ++it;
    EXPECT_TRUE(it->is_free());
    EXPECT_EQ(it->max_data_size(), alloc_size);
    ++it;
    EXPECT_TRUE(it->is_used());
    EXPECT_EQ(it->data_size(), alloc_size);
}

void expect_used(const Block &b, uint32_t size,
                 uint32_t data_size = TS_ALLOC_SIZE) {
    EXPECT_TRUE(b.is_used());
    EXPECT_EQ(b.size(), size);
    EXPECT_EQ(b.data_size(), data_size);
}

void expect_free(const Block &b, uint32_t size) {
    EXPECT_TRUE(b.is_free());
    EXPECT_EQ(b.size(), size);
}

uint32_t make_n(TupleList &ts, size_t n) {
    auto it = ts.iter();
    it.init();
    uint32_t size = 0;
    for (size_t i = 0; i < n; ++i) {
        it.alloc(TS_ALLOC_SIZE);
        size = it->size();
        ++it;
    }
    return size;
}

uint32_t make_five(TupleList &ts) { return make_n(ts, 5); }

TEST(TupleSpace, FreeMergeLeft) {
    auto ts = make_list();
    uint32_t size = make_five(ts);
    auto it = ts.iter();
    it.init();
    ++it;
    it.free();
    ++it;
    it.free();
    it.rewind();

    expect_used(*it, size);
    ++it;
    expect_free(*it, 2 * size);
    ++it;
    expect_used(*it, size);
    ++it;
    expect_used(*it, size);
}

TEST(TupleSpace, FreeMergeRight) {
    auto ts = make_list();
    uint32_t size = make_five(ts);
    auto it = ts.iter();
    it.init();
    ++it;
    ++it;
    ++it;
    it.free();
    it.rewind();

    ++it;
    ++it;
    it.free();
    it.rewind();

    expect_used(*it, size);
    ++it;
    expect_used(*it, size);
    ++it;
    expect_free(*it, 2 * size);
    ++it;
    expect_used(*it, size);
}

TEST(TupleSpace, FreeMergeBoth) {
    auto ts = make_list();
    uint32_t size = make_five(ts);
    auto it = ts.iter();
    it.init();
    ++it;
    it.free();
    ++it;
    ++it;
    it.free();
    it.rewind();

    ++it;
    ++it;
    it.free();
    it.rewind();

    expect_used(*it, size);
    ++it;
    expect_free(*it, 3 * size);
    ++it;
    expect_used(*it, size);
}

TEST(TupleSpace, MultipleMerges) {
    auto ts = make_list();
    constexpr size_t n = 16;
    uint32_t size = make_n(ts, n);
    auto it = ts.iter();
    it.init();
    for (size_t i = 0; i < n; ++i, ++it) {
        if (i % 4 == 0)
            it.free();
    }
    it.rewind();

    for (size_t i = 0; i < n; ++i, ++it) {
        if (i % 4 == 0)
            expect_free(*it, size);
        else
            expect_used(*it, size);
    }
    it.rewind();

    for (size_t i = 0; i < n; ++i) {
        if (i % 4 == 3)
            it.free();
        else // block merge means one less block
            ++it;
    }
    it.rewind();

    expect_free(*it, size);
    ++it;
    expect_used(*it, size);
    ++it;
    expect_used(*it, size);
    ++it;

    for (size_t i = 0; i < 3; ++i) {
        expect_free(*it, 2 * size);
        ++it;
        expect_used(*it, size);
        ++it;
        expect_used(*it, size);
        ++it;
    }
    // The last freed was merged into the big free block
    // expect_free(*it, 2 * size);
    // ++it;
}
