#include "LimitedDataQueue.h"
#include "ThreadPool.h"
#include "Timer.h"
#include <atomic>
#include <chrono>
#include <cstdio>
#include <cstring>
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
        auto b = qq->wait_pop(a);
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
        auto b = qq->wait_pop(a);
        if (b) {
            sum += a;
            std::cout << "summing " << a << '\n';
        }
    }
    return sum;
}

// template <typename T>
// class Worker {
//   private:
//     mutable std::mutex m_;
//     std::vector<T> buffer_;
//     std::condition_variable data_available_;

//   public:
//     Worker(size_t buffer_size) : buffer_(buffer_size) {}
//     T *data() {
//         std::unique_lock<std::mutex> lock(m_);
//         return buffer_.data();
//     }

//     size_t size() {
//         std::unique_lock<std::mutex> lock(m_);
//         return buffer_.size();
//     }

//     void print() {
//         for (auto &item : buffer_) {
//             printf("%d\n", item);
//         }
//     }
//     void process() {
//         std::unique_lock<std::mutex> lock(m_);
//         while (true) {
//             printf("waiting for data\n");
//             data_available_.wait(lock);
//             print();
//         }
//     }
//     void notify() {
//         std::unique_lock<std::mutex> lock(m_);
//         data_available_.notify_all();
//     }
// };

template <typename T>
class ThreadSafeReader {
  private:
    mutable std::mutex m_;
    size_t count_;
    int value_ = 0;

  public:
    ThreadSafeReader(size_t count) : count_(count) {}
    size_t read(T *dest) {
        std::unique_lock<std::mutex> lock(m_);
        std::vector<T> data(count_, value_++);
        std::memcpy(dest, data.data(), count_ * sizeof(T));
        return count_;
    }
};

template <typename T>
void print(T containter) {
    for (auto &item : containter) {
        printf("%d\n", item);
    }
}

void process(ThreadSafeReader<int> *rr, int id) {
    std::vector<int> vec(5);
    for (int i = 0; i != 10; ++i) {
        rr->read(vec.data());
        printf("thread: %d\n", id);
        print(vec);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

int main() {


    auto l = [](){};
    // ThreadSafeReader<int> r{5};
    // std::vector<std::thread> threads;
    // for (int i = 0; i != 4; ++i) {
    //     threads.push_back(std::thread(process, &r, i));
    // }


    // for (auto &t : threads) {
    //     t.join();
    // }
    // size_t max_size = 10;
    // auto q = LimitedDataQueue<double>{max_size};

    // auto f = std::async(std::launch::async, func, &q);

    // std::vector<double> vec{1., 2., 3., 4., 5.};
    // for (auto item : vec) {
    //     std::cout << "pushing " << item << '\n';
    //     q.wait_push(item);
    // }
    // q.setProducerFinished(true);
    // auto a = f.get();
    // std::cout << "sum is: " << a << '\n';

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
    //     q.wait_push(values);
    // }
    // q.setProducerFinished(true);

    // for (auto &t : threads) {
    //     t.join();
    // }
    // t.print_elapsed();
}