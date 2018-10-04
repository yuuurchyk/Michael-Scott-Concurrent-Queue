#ifndef CONCURRENT_QUEUE_EXTENDED_H_
#define CONCURRENT_QUEUE_EXTENDED_H_

#include <iostream>
#include <new>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <string>
#include <exception>

#include "../ConcurrentQueueBase/ConcurrentQueueBase.hpp"

template<class T>
class ConcurrentQueueExtended final: public ConcurrentQueueBase<T>{
public:
    ConcurrentQueueExtended():
        lB(new Block(T())),
        rB(lB)
    {}

    std::string description() const override { return "ConcurrentQueueExtended"; }

    std::recursive_mutex& getPopMutex() override{ return head; }

    void lock() const override{ std::lock(head, tail); }
    void unlock() const override {
        head.unlock();
        tail.unlock();
    };
    bool try_lock() const override{ return std::try_lock(head, tail) == -1; }

    size_t size() const override{
        std::unique_lock<const ConcurrentQueueExtended<T>> lck(*this);
        return size_;
    }
    bool empty() const override{ return size() == 0; }

    void push(const T &val) override{
        std::lock_guard<std::recursive_mutex> lck(tail);
        if(rB->rI == rB->kSize - 1){
            rB->rightSibling = new Block(val);
            ++rB->isRightSibling;
            rB = rB->rightSibling;
        }
        else{
            new(rB->data + rB->rI + 1) T(val);
            ++rB->rI;
        }
        ++size_;
    }

    bool tryPop(T *target=nullptr) override{
        std::lock_guard<std::recursive_mutex> lck(head);
        if(lB->lI == lB->kSize - 1){
            if(!lB->isRightSibling)
                return false;
            auto old = lB;
            lB = lB->rightSibling;
            delete old;
            if(target != nullptr)
                *target = *lB->data;
            --size_;
            return true;
        }
        else{
            if(lB->lI + 1 > lB->rI)
                return false;
            (lB->data + lB->lI)->~T();
            if(target != nullptr)
                *target = *(lB->data + lB->lI + 1);
            ++lB->lI;
            --size_;
            return true;
        }
        return false;
    }

    T pop() override{
        T res;

        std::unique_lock<std::recursive_mutex> lck(head);

        while(true)
            if(tryPop(&res))
                return res;
    }

    virtual ~ConcurrentQueueExtended(){
        while(lB != nullptr){
            auto old = lB;
            lB = lB->rightSibling;
            delete old;
        }
    }

private:
    class Block{
    public:
        static constexpr size_t kSize{10000};

        T* data{nullptr};
        std::atomic<size_t> lI{0}, rI{0};

        std::atomic<char> isRightSibling{false};
        Block *rightSibling{nullptr};
        
        Block(const T &val):
            data(static_cast<T*>(::operator new(sizeof(T) * kSize)))
        {
            new(data) T(val);
        }

        ~Block(){
            while(lI <= rI)
                (data + (lI++))->~T();
            ::operator delete(data);
        }
    };

    std::atomic<size_t> size_{0};

    mutable std::recursive_mutex head{}, tail{};
    
    Block *lB{nullptr}, *rB{nullptr};
};

#endif