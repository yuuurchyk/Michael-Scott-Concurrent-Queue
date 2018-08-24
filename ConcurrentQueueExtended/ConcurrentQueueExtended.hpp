#ifndef CONCURRENT_QUEUE_EXTENDED_H_
#define CONCURRENT_QUEUE_EXTENDED_H_

#include <new>
#include <mutex>
#include <condition_variable>
#include <atomic>

#include "../ConcurrentQueueBase/ConcurrentQueueBase.hpp"

template <class T>
class ConcurrentQueueExtended final: public ConcurrentQueueBase<T>{
public:
    ConcurrentQueueExtended():
        leftMostBlock(new Block(T())),
        rightMostBlock(leftMostBlock)
    {}

    std::string description() const override{ return "ConcurrentQueueExtended"; }

    std::recursive_mutex& getPopMutex() override{ return head; }
    std::condition_variable_any& getCondVar() override { return condVar; }

    void lock() const override{ std::lock(head, tail); }
    void unlock() const override {
        head.unlock();
        tail.unlock();
    };
    bool try_lock() const override{ return std::try_lock(head, tail) == -1; }

    void push(const T &val) override{
        std::lock_guard<std::recursive_mutex> lck(tail);

        if(
            rightMostBlock->rightElement + 1 < 
            rightMostBlock->data + rightMostBlock->kBlockSize
        )
            new(++rightMostBlock->rightElement) T(val);
        else{
            rightMostBlock->rightBlock = new Block(val);
            rightMostBlock = rightMostBlock->rightBlock;
        }

        condVar.notify_one();
    }

    T pop() override{
        T res;
        while(!tryPop(&res)){}
        return res;
    }
    bool tryPop(T *target=nullptr) override{
        std::lock_guard<std::recursive_mutex> lck(head);

        if(leftMostBlock->leftElement == leftMostBlock->data + leftMostBlock->kBlockSize - 1){
            if(leftMostBlock->rightBlock == nullptr)
                return false;
            
            Block *oldBlock = leftMostBlock;
            leftMostBlock = leftMostBlock->rightBlock;
            delete oldBlock;
            *target = *(leftMostBlock->leftElement);
            return true;
        }
        else{
            if(leftMostBlock->leftElement == leftMostBlock->rightElement)
                return false;
            *target = *(leftMostBlock->leftElement + 1);
            (leftMostBlock->leftElement++)->~T();
            return true;
        }

        return false;
    }

    T& front() override{
        std::lock_guard<ConcurrentQueueExtended<T>> lck(*this);

        if(leftMostBlock->leftElement == leftMostBlock->data + leftMostBlock->kBlockSize - 1)
            return *(leftMostBlock->rightBlock->leftElement);
        else
            return *(leftMostBlock->leftElement + 1);
    }
    const T& front() const override{
        return front();
    }

    bool empty() const override{
        return size() == 0;
    }
    size_t size() const override{
        std::lock_guard<const ConcurrentQueueExtended<T>> lck(*this);

        if(leftMostBlock == rightMostBlock)
            return (leftMostBlock->rightElement - rightMostBlock->leftElement + 1) - 1;
        
        size_t res = (rightMostBlock->id - leftMostBlock->id + 1 - 2) * leftMostBlock->kBlockSize;
        res += (leftMostBlock->data + leftMostBlock->kBlockSize - 1 - leftMostBlock->leftElement + 1);
        res += (rightMostBlock->rightElement - rightMostBlock->data + 1);

        return res - 1;
    }

    ~ConcurrentQueueExtended(){
        while(leftMostBlock != nullptr){
            auto oldBlock = leftMostBlock;
            leftMostBlock = leftMostBlock->rightBlock;
            delete oldBlock;
        }
    }
private:
    static size_t blockIdCounter;

    class Block{
    public:
        static constexpr size_t kBlockSize{10000};

        size_t id;
        T *data{nullptr};
        T *leftElement{nullptr}, *rightElement{nullptr};
        Block *rightBlock{nullptr};

        Block(const T &val):
            id(++blockIdCounter),
            data(static_cast<T*>(::operator new(sizeof(T) * kBlockSize))),
            leftElement(data),
            rightElement(data)
        {
            new(leftElement) T(val);
        }
        
        ~Block(){
            while(leftElement <= rightElement)
                (leftElement++)->~T();
            ::operator delete(data);
        }
    };

    mutable std::recursive_mutex head{}, tail{};
    std::condition_variable_any condVar;
    Block *leftMostBlock{nullptr}, *rightMostBlock{nullptr};
};

template<class T>
size_t ConcurrentQueueExtended<T>::blockIdCounter{0};

#endif