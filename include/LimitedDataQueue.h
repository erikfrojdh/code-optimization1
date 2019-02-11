#pragma once

#include <condition_variable>
#include <cstdlib>
#include <iostream>
#include <mutex>
#include <queue>

template <typename T>
class LimitedDataQueue {
    std::queue<T> queue_;
    mutable std::mutex m_;
    std::condition_variable data_available_;
    std::condition_variable space_available_;
    size_t capacity_ = 10;
    bool producer_finished_ = false;

    LimitedDataQueue &operator=(const LimitedDataQueue &) = delete;
    LimitedDataQueue(const LimitedDataQueue &other) = delete;

  public:
    LimitedDataQueue(){}
    LimitedDataQueue(size_t capacity) : capacity_(capacity) {}

    bool producerNotFinished() const {
        std::unique_lock<std::mutex> lock(m_);
        return !producer_finished_;
    }

    void setProducerFinished(bool value) {
        std::unique_lock<std::mutex> lock(m_);
        while (!queue_.empty()) {
            space_available_.wait(lock);
        }
        producer_finished_ = value;
        space_available_.notify_all();
        data_available_.notify_all();
    }

    size_t capacity() const {
        return capacity_;
    }

    void set_capacity(size_t c) {
        capacity_ = c;
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(m_);
        return queue_.empty();
    }

    size_t size() const {
        std::lock_guard<std::mutex> lock(m_);
        return queue_.size();
    }

    bool try_push(T item){
        std::unique_lock<std::mutex> lock(m_);
        if (queue_.size()< capacity_){
            queue_.push(std::move(item));
            data_available_.notify_one();
            return true;
        }else{
            return false;
        }
    }


    /*blocking push  try to push data on to the queue
    if full wait for space to be available*/
    void wait_and_push(T item) {
        std::unique_lock<std::mutex> lock(m_);
        while (queue_.size() > capacity_) {
            space_available_.wait(lock);
        }
        queue_.push(std::move(item));
        data_available_.notify_one();
    }

    /*blocking_pop, try to pop an item from the 
    queue and if the queue is empty wait for new data
    or producerFinished. Returns true if successful
    false if the queue was empty and producerFinished*/
    bool wait_and_pop(T &popped_item) {
        std::unique_lock<std::mutex> lock(m_);
        while (queue_.empty() && !producer_finished_) {
            data_available_.wait(lock);
        }

        if (!queue_.empty()) {
            popped_item = std::move(queue_.front());
            queue_.pop();
            space_available_.notify_one(); 
            return true;
        }
        return false;
    }
};