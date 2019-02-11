#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include <vector>
#include "LimitedDataQueue.h"
#include "ThreadPool.h"

#include <cstdio>

void consumer(LimitedDataQueue<int> *qq, int id, std::atomic<bool>& done)
{
    std::cout << "Consumer start\n";
    while (!done)
    {
        int a = 0;
        qq->wait_and_pop(a);
        std::this_thread::sleep_for(std::chrono::seconds(1));
        printf("Thread: %d got %d\n", id, a);
    }
    return;
}

int main()
{

    // auto pool = ThreadPool<int>(5);

    // std::cout << "Done\n";
    // std::cout << "Hej\n";

    std::atomic<bool> shutdown = false;
    auto q = LimitedDataQueue<int>();
    q.set_capacity(3);


    // // //Using a vector for thread pool
    std::vector<std::thread> tvec;
    for (int i=0; i!=4; ++i){
        tvec.push_back(std::thread(consumer, &q, i, std::ref(shutdown)));
    }


    for (int i = 0; i != 10; ++i)
    {
        q.wait_and_push(i);
    }

    // shutdown = true;
    // // q.push(99);
    // // q.push(99);
    for(auto& t:tvec){
        t.join();
    }

    // std::cout << "size: " << q.size() << "\n";
}