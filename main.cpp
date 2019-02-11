#include "LimitedDataQueue.h"
#include "ThreadPool.h"
#include "Timer.h"
#include <atomic>
#include <chrono>
#include <cstdio>
#include <future>
#include <iostream>
#include <numeric>
#include <thread>
#include <vector>

using queue_type = std::vector<int>;

void consumer(LimitedDataQueue<queue_type> *qq, int id) {
    std::cout << "Consumer start\n";
    while (qq->producerNotFinished()) {
        queue_type a;
        auto b = qq->wait_and_pop(a);
        if (b) {
            auto sum = std::accumulate(a.begin(), a.end(), 0);
            printf("Thread %d: sum %d\n", id, sum);
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
    }
    printf("Thread: %d is done\n", id);
    return;
}

double func(LimitedDataQueue<double> *qq) {
    std::cout << "I'm func\n";
    double sum = 0;
    while (qq->producerNotFinished()) {
        double a = 0;
        auto b = qq->wait_and_pop(a);
        if (b) {
            sum += a;
            std::cout << "summing " << a << '\n';
        }
    }
    return sum;
}

int main() {

    size_t max_size = 10;
    auto q = LimitedDataQueue<double>{max_size};

    auto f = std::async(std::launch::async, func, &q);

    std::vector<double> vec{1., 2., 3., 4., 5.};
    for (auto item : vec) {
        std::cout << "pushing " << item << '\n';
        q.wait_and_push(item);
    }
    q.setProducerFinished(true);
    auto a = f.get();
    std::cout << "sum is: " << a << '\n';

    // sls::Timer t;
    // auto q = LimitedDataQueue<queue_type>(5);
    // std::cout << "Capacity: " << q.capacity() << '\n';

    // std::vector<std::thread> threads;
    // for (int i = 0; i != 2; ++i) {
    //     threads.push_back(std::thread(consumer, &q, i));
    // }

    // for (int i = 1; i != 21; ++i) {
    //     std::vector<int> values(i);
    //     for (int j = 0; j != values.size(); ++j) {
    //         values[j] = j;
    //     }
    //     // std::this_thread::sleep_for(std::chrono::milliseconds(100));
    //     q.wait_and_push(values);
    // }
    // q.setProducerFinished(true);

    // for (auto &t : threads) {
    //     t.join();
    // }
    // t.print_elapsed();
}