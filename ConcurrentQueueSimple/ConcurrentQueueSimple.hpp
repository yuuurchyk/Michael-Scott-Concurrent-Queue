#ifndef CONCURRENT_QUEUE_SIMPLE_H_
#define CONCURRENT_QUEUE_SIMPLE_H_

#include <queue>
#include <iostream>

#include "../ConcurrentQueueBase/ConcurrentQueueBase.hpp"

template <class T>
class ConcurrentQueueSimple final: public ConcurrentQueueBase<T>{
public:
    std::string description() const override { return "ConcurrentDequeSimple"; }

    std::recursive_mutex& getPopMutex() override { return mux_; }
    std::condition_variable_any& getCondVar() override { return condVar_; }

    void lock() const override { mux_.lock(); }
    void unlock() const override { mux_.unlock(); }
    bool try_lock() const override { return mux_.try_lock(); }

    void push(const T &val) override {
        std::lock_guard<std::recursive_mutex> lck(mux_);
        q_.push(val);
        condVar_.notify_one();
    }

    T pop() override {
        std::unique_lock<std::recursive_mutex> lck(mux_);

        while(empty())
            condVar_.wait(lck);

        T res = q_.front();
        q_.pop();

        condVar_.notify_one();

        return res;
    }
    bool tryPop(T *target=nullptr) override {
        std::lock_guard<std::recursive_mutex> lck(mux_);

        if(empty())
            return false;
        
        if(target != nullptr)
            *target = q_.front();
        
        q_.pop();

        condVar_.notify_one();

        return true;
    }

    T& front() override{
        std::lock_guard<std::recursive_mutex> lck(mux_);
        return q_.front();
    }
    const T& front() const override{ return front(); }

    bool empty() const override{
        std::lock_guard<std::recursive_mutex> lck(mux_);
        return q_.empty();
    }
    size_t size() const override{
        std::lock_guard<std::recursive_mutex> lck(mux_);
        return q_.size();
    }

    virtual ~ConcurrentQueueSimple() = default;
private:
    mutable std::recursive_mutex mux_{};
    std::condition_variable_any condVar_;
    std::queue<T> q_{};
};

#endif