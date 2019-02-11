#include "LimitedDataQueue.h"
#include "catch/catch.hpp"

#include <future>

TEST_CASE("default constructor and initial state") {

    auto q = LimitedDataQueue<int>{};
    CHECK(q.empty());
    CHECK(q.size() == 0);
    CHECK(q.capacity() == 10);
}

TEST_CASE("basic push and pop") {

    auto q = LimitedDataQueue<int>{};
    q.wait_and_push(5);
    CHECK(q.size() == 1);
    CHECK(q.capacity() == 10);

    int i = 0;
    auto b = q.wait_and_pop(i);

    CHECK(b);
    CHECK(i == 5);
    CHECK(q.empty());
    CHECK(q.capacity() == 10);

    q.wait_and_push(6);
    q.wait_and_push(7);
    CHECK(q.size() == 2);

    b = q.wait_and_pop(i);
    CHECK(b);
    CHECK(q.size() == 1);
    CHECK(i == 6);

    q.wait_and_push(8);
    b = q.wait_and_pop(i);
    CHECK(i == 7);
    b = q.wait_and_pop(i);
    CHECK(i == 8);
    CHECK(q.empty());
}

TEST_CASE("push to capacity") {
    size_t max_size = 3;
    auto q = LimitedDataQueue<int>{max_size};
    std::vector<int> vec{0, 1, 2};
    bool b;
    for (auto item : vec) {
        b = q.try_push(item);
        CHECK(b);
    }
    CHECK(q.size() == max_size);

    b = q.try_push(3);
    CHECK(b == false);
    CHECK(q.size() == max_size);

    int i;
    b = q.wait_and_pop(i);
    CHECK(b);
    CHECK(i == 0);
    b = q.wait_and_pop(i);
    CHECK(b);
    CHECK(i == 1);
    b = q.wait_and_pop(i);
    CHECK(b);
    CHECK(i == 2);
}

double func(LimitedDataQueue<double> *qq) {
    double sum = 0;
    while (qq->producerNotFinished()) {
        double a = 0;
        auto b = qq->wait_and_pop(a);
        if (b) {
            sum += a;
        }
    }
    return sum;
}

TEST_CASE("summation in thread using async") {

    size_t max_size = 3;
    auto q = LimitedDataQueue<double>{max_size};

    auto f = std::async(std::launch::async, func, &q);

    std::vector<double> vec{1., 2., 3., 4., 5.};
    for (auto item : vec) {
        q.wait_and_push(item);
    }
    q.setProducerFinished(true);
    CHECK(f.get() == 15);
}