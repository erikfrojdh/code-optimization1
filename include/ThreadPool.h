#include "LimitedDataQueue.h"
#include <thread>



template <typename T>
class ThreadPool
{
  private:
    std::vector<std::thread> workers_;
    LimitedDataQueue<T> queue_{};

  public:
    ThreadPool(int n_worker)
    {

    }
};