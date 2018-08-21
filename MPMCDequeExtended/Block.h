#ifndef BLOCK_H_
#define BLOCK_H_

#include <mutex>
#include <new>
#include <stdexcept>
#include <memory>

enum class BlockPolicy{middle, left, right};

template<class T>
class Block{
public:
    Block(const BlockPolicy &policy):
        data(getMemory())
    {
        switch(policy){
            case BlockPolicy::left:
                leftElement = data + kBlockSize;
                rightElement = data + kBlockSize - 1;
                break;
            case BlockPolicy::right:
                leftElement = data;
                rightElement = data - 1;
                break;
            case BlockPolicy::middle:
                leftElement = data + kBlockSize / 2;
                rightElement = leftElement - 1;
                break;
        }
    }

    T *data{nullptr};
    T *leftElement{nullptr}, *rightElement{nullptr};

    void lock(){
        mux.lock();
    }
    void unlock(){
        mux.unlock();
    }

    std::shared_ptr<Block<T>> leftBlock{nullptr}, rightBlock{nullptr};

    ~Block(){
        if(leftBlock != nullptr)
            leftBlock->rightBlock = nullptr;
        if(rightBlock != nullptr)
            rightBlock->rightBlock = nullptr;

        leftBlock = nullptr;
        rightBlock = nullptr;

        while(leftElement <= rightElement){
            leftElement->~T();
            ++leftElement;
        }

        ::operator delete(data);
    }

    bool fullLeft(){
        return leftElement == data;
    }
    bool fullRight(){
        return rightElement == data + kBlockSize - 1;
    }

    bool empty(){
        return leftElement > rightElement;
    }
private:
    static constexpr size_t kBlockSize{10000};
    std::mutex mux{};

    static T* getMemory(){
        T *result = static_cast<T*>(::operator new(sizeof(T) * kBlockSize, std::nothrow));
        if(result == nullptr)
            throw std::overflow_error("Cannot allocate memory");
        return result;
    }
};

#endif