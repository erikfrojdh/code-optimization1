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
    q.wait_push(5);
    CHECK(q.size() == 1);
    CHECK(q.capacity() == 10);

    int i = 0;
    auto b = q.wait_pop(i);

    CHECK(b);
    CHECK(i == 5);
    CHECK(q.empty());
    CHECK(q.capacity() == 10);

    q.wait_push(6);
    q.wait_push(7);
    CHECK(q.size() == 2);

    b = q.wait_pop(i);
    CHECK(b);
    CHECK(q.size() == 1);
    CHECK(i == 6);

    q.wait_push(8);
    b = q.wait_pop(i);
    CHECK(i == 7);
    b = q.wait_pop(i);
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
    b = q.wait_pop(i);
    CHECK(b);
    CHECK(i == 0);
    b = q.wait_pop(i);
    CHECK(b);
    CHECK(i == 1);
    b = q.wait_pop(i);
    CHECK(b);
    CHECK(i == 2);
}

double func(LimitedDataQueue<double> *qq) {
    double sum = 0;
    while (qq->producerNotFinished()) {
        double a = 0;
        auto b = qq->wait_pop(a);
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
        q.wait_push(item);
    }
    q.setProducerFinished(true);
    CHECK(f.get() == 15);
}

TEST_CASE("pop on empty and push on full"){
    using dtype = std::vector<float>;
    size_t max_size = 50;
    auto q = LimitedDataQueue<dtype>{max_size};

    for (int i = 0; i!=max_size; ++i){
        dtype data{5., 7.5, 9.3, 5000.};
        auto b = q.try_push(data);
        CHECK(b == true);
        CHECK(q.size() == i+1);
    }
    CHECK(q.size() == max_size);


    auto r0 = q.try_push(dtype{5.});
    CHECK(r0 == false);
    CHECK(q.size() == max_size);

    for (int i=0; i!=max_size; ++i){
        dtype vec;
        auto b = q.try_pop(vec);
        CHECK(b == true);
        CHECK(q.size() == max_size-1-i);
    }

    dtype d;
    auto r1 = q.try_pop(d);
    CHECK(r1 == false);
    CHECK(q.size() == 0);

}