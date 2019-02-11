#include "LimitedDataQueue.h"
#include "ThreadPool.h"
#include "Timer.h"
#include <atomic>
#include <chrono>
#include <cstdio>
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

int main() {


    sls::Timer t;
    auto q = LimitedDataQueue<queue_type>(5);


    std::vector<std::thread> threads;
    for (int i = 0; i != 2; ++i) {
        threads.push_back(std::thread(consumer, &q, i));
    }

    for (int i = 1; i != 21; ++i) {
        std::vector<int> values(i);
        for (int j = 0; j != values.size(); ++j) {
            values[j] = j;
        }
        // std::this_thread::sleep_for(std::chrono::milliseconds(100));
        q.wait_and_push(values);
    }
    q.setProducerFinished(true);


    for (auto &t : threads) {
        t.join();
    }
    t.print_elapsed();
}