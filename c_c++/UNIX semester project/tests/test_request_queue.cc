#include <gtest/gtest.h>

#include "../src/RequestQueue.h"

TEST(RequestQueueTests, SimpleQueueTests) {
    int zeroth, first, second, third;

    // simulating external buffer
    RequestQueue initial(ftok(".", 'T'));
    char buffer[sizeof(RequestQueue)];
    *(RequestQueue *)buffer = initial;
    // separate object, client perspecrive
    RequestQueue &rq = *(RequestQueue *)buffer;
    Request req1 = {1, 1, 1};
    Request req2 = {2, 1, 2};
    Request req3 = {3, 1, 3};
    ASSERT_EQ(rq.getFirst(), INDEXNULL);
    ASSERT_EQ(rq.getNElements(), 0);

    // one element
    rq.addRequest(req1);
    first = rq.getFirst();
    ASSERT_EQ(rq.getElement(first)->request.fresh, 1);
    ASSERT_EQ(rq.getElement(first)->request.notifyingSemaphoreKey, 1);
    ASSERT_EQ(rq.getElement(first)->next, INDEXNULL);
    ASSERT_EQ(rq.getElement(first)->prev, INDEXNULL);
    ASSERT_EQ(rq.getElement(first)->active, true);
    const RequestQueueElement *oldPtr = rq.getElement(first);

    // zero again
    rq.removeRequest(first);
    ASSERT_EQ(rq.getNElements(), 0);
    ASSERT_EQ(rq.getFirst(), INDEXNULL);
    ASSERT_EQ(oldPtr->active, false);

    // two elements
    rq.addRequest(req3);
    rq.addRequest(req2);
    first = rq.getFirst();
    second = rq.getElement(first)->next;
    ASSERT_EQ(rq.getElement(first)->request.fresh, 3);
    ASSERT_EQ(rq.getElement(second)->request.fresh, 2);
    ASSERT_EQ(rq.getElement(second)->next, INDEXNULL);
    ASSERT_EQ(rq.getElement(second)->prev, first);
    ASSERT_EQ(rq.getElement(first)->prev, INDEXNULL);
    ASSERT_EQ(rq.getNElements(), 2);

    // 3 elements
    rq.addRequest(req1);
    first = rq.getFirst();
    second = rq.getElement(first)->next;
    third = rq.getElement(second)->next;
    ASSERT_EQ(rq.getElement(first)->request.fresh, 3);
    ASSERT_EQ(rq.getElement(second)->request.fresh, 2);
    ASSERT_EQ(rq.getElement(third)->request.fresh, 1);
    ASSERT_EQ(rq.getElement(second)->prev, first);
    ASSERT_EQ(rq.getNElements(), 3);

    // remove middle one
    rq.removeRequest(second);
    first = rq.getFirst();
    second = rq.getElement(first)->next;
    ASSERT_EQ(rq.getNElements(), 2);
    ASSERT_EQ(rq.getElement(first)->request.fresh, 3);
    ASSERT_EQ(rq.getElement(second)->request.fresh, 1);
    ASSERT_EQ(rq.getElement(second)->prev, first);

    // remove first
    rq.removeRequest(rq.getFirst());
    first = rq.getFirst();
    ASSERT_EQ(rq.getNElements(), 1);
    ASSERT_EQ(rq.getElement(first)->request.notifyingSemaphoreKey, 1);
    ASSERT_EQ(rq.getElement(first)->next, INDEXNULL);
    ASSERT_EQ(rq.getElement(first)->prev, INDEXNULL);
    ASSERT_EQ(rq.getElement(first)->active, true);

    // empty again
    rq.removeRequest(rq.getFirst());
    ASSERT_EQ(rq.getNElements(), 0);
    ASSERT_EQ(rq.getFirst(), INDEXNULL);
}

TEST(RequestQueueTests, QueueCapTest) {
    Request dummy = {1, 1};
    RequestQueue rq(ftok(".", 'T'));
    for (int i = 0; i < RequestQueue::QUEUE_SIZE; ++i) {
        rq.addRequest(dummy);
    }
    int one_less_than_full = RequestQueue::QUEUE_SIZE - 1;
    int full = RequestQueue::QUEUE_SIZE;

    ASSERT_EQ(rq.getNElements(), full);
    ASSERT_EQ(rq.addRequest(dummy), INDEXNULL);

    int first = rq.getFirst();
    int second = rq.getElement(first)->next;
    int third = rq.getElement(second)->next;
    rq.removeRequest(third);
    ASSERT_EQ(rq.getNElements(), one_less_than_full);
    rq.addRequest(dummy);
    ASSERT_EQ(rq.getNElements(), full);
}

TEST(RequestQueueTests, QueueTestEmpty) {
    RequestQueue rq(ftok(".", 'T'));
    Request dummy = {1, 1};
    rq.addRequest(dummy);
    int old = rq.getFirst();
    rq.removeRequest(old);
    ASSERT_THROW(rq.removeRequest(old), std::invalid_argument);
    ASSERT_THROW(rq.removeRequest(INDEXNULL), std::invalid_argument);
}

TEST(RequestQueueTests, QueueSemaphores) {
    // simulating external buffer
    RequestQueue initial(ftok(".", 'T'));
    char buffer[sizeof(RequestQueue)];
    *(RequestQueue *)buffer = initial;
    // separate object, client perspective
    RequestQueue &rq = *(RequestQueue *)buffer;

    rq.lock();
    rq.unlock();
}

TEST(RequestQueueTests, DifferentAdressesTest) {
    RequestQueue rq(ftok(".", 'T'));
    rq.addRequest({1, 2});

    char buffer1[sizeof(RequestQueue)];
    char buffer2[sizeof(RequestQueue)];

    *(RequestQueue *)buffer1 = rq;
    memcpy(buffer2, buffer1, sizeof(RequestQueue));

    auto handle1 = (RequestQueue *)buffer1;
    auto handle2 = (RequestQueue *)buffer2;

    ASSERT_EQ(handle1->getFirst(), handle2->getFirst());
    ASSERT_NE(handle1->getElement(handle1->getFirst()),
              handle2->getElement(handle2->getFirst()));
    ASSERT_NE(handle1->getBuf(), handle2->getBuf());
}