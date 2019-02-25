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

#include "Histogram2D.h"

int main() {

    Histogram2D<int, double> h(10, 0, 25);

    h.Fill(5);
    h.Fill(6.1);
    h.Print();
}