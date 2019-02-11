#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>

template <typename T>
class LimitedDataQueue
{
    std::queue<T> queue_;
    mutable std::mutex m_;
    std::condition_variable data_cond_;
    // std::condition_variable popped_cond_;
    size_t capacity_ = 10;
    LimitedDataQueue &operator=(const LimitedDataQueue &) = delete;
    LimitedDataQueue(const LimitedDataQueue &other) = delete;

  public:
    LimitedDataQueue() {}

    size_t capacity() const{
        return capacity_;
    }
    
    void set_capacity(size_t c){
        capacity_ = c;
    }
    // void push(T item)
    // {
    //     {
    //         std::lock_guard<std::mutex> lock(m_);
    //         queue_.push(std::move(item));
    //     }
    //     data_cond_.notify_one();
    // }

    void wait_and_push(T item)
    {
        std::unique_lock<std::mutex> lock(m_);
        while (queue_.size() > capacity_)
        {
            // std::cout << "Size: " << queue_.size() << '\n';
            popped_cond_.wait(lock);

            //  This 'while' loop is equal to
            //  data_cond_.wait(lock, [](bool result){return !queue_.empty();});
        }
        queue_.push(std::move(item));
        data_cond_.notify_one();
    }

    bool empty() const
    {
        std::lock_guard<std::mutex> lock(m_);
        return queue_.empty();
    }

    size_t size() const
    {
        std::lock_guard<std::mutex> lock(m_);
        return queue_.size();
    }

    // bool try_and_pop(T &popped_item)
    // {
    //     std::lock_guard<std::mutex> lock(m_);
    //     if (queue_.empty())
    //     {
    //         return false;
    //     }
    //     popped_item = std::move(queue_.front());
    //     queue_.pop();
    //     return true;
    // }

    void wait_and_pop(T &popped_item)
    {
        std::unique_lock<std::mutex> lock(m_);
        while (queue_.empty())
        {
            data_cond_.wait(lock);
            //  This 'while' loop is equal to
            //  data_cond_.wait(lock, [](bool result){return !queue_.empty();});
        }
        popped_item = std::move(queue_.front());
        queue_.pop();
        popped_cond_.notify_one();
    }
};