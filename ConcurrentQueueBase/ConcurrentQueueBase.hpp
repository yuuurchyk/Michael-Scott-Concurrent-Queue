#ifndef CONCURRENT_QUEUE_BASE_H_
#define CONCURRENT_QUEUE_BASE_H_

#include <ctype.h>
#include <string>
#include <mutex>
#include <condition_variable>

template <class T>
class ConcurrentQueueBase{
public:
    virtual std::string description() const = 0;

    virtual std::recursive_mutex& getPopMutex() = 0;
    virtual std::condition_variable_any& getCondVar() = 0;

    virtual void lock() const = 0;
    virtual void unlock() const = 0;
    virtual bool try_lock() const = 0;

    virtual void push(const T &val) = 0;

    virtual T pop() = 0;
    virtual bool tryPop(T *target=nullptr) = 0;

    virtual bool empty() const = 0;
    virtual size_t size() const = 0;

    virtual ~ConcurrentQueueBase() = default;
};

#endif