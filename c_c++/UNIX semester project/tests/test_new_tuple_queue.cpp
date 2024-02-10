#include "../src/new_tuple_queue.hpp"
#include <gtest/gtest.h>

using namespace ts;

static constexpr uint32_t TS_QUEUE_SIZE = 128;

NewTupleQueue make_queue() {
    key_t key = 0xDEADBEEF;
    return NewTupleQueue(key, "/tuple-space-new-tuple-queue-test", true, true,
                         TS_QUEUE_SIZE);
}

TEST(NewTupleQueue, AddTakeOne) {
    auto q = make_queue();
    q.add(1234);
    auto v = q.take();
    EXPECT_EQ(v, 1234);
}

TEST(NewTupleQueue, AddTakeMultiple) {
    auto q = make_queue();
    q.add(111);
    q.add(222);
    q.add(333);
    EXPECT_EQ(q.take(), 111);
    EXPECT_EQ(q.take(), 222);
    EXPECT_EQ(q.take(), 333);
}

TEST(NewTupleQueue, AddTakeTillFull) {
    auto q = make_queue();
    for (uint32_t i = 0; i < TS_QUEUE_SIZE; ++i) {
        q.add(i);
    }
    for (uint32_t i = 0; i < TS_QUEUE_SIZE; ++i) {
        EXPECT_EQ(q.take(), i);
    }
}
