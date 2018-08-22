#ifndef MPMCDEQUEEXTENDED_H_
#define MPMCDEQUEEXTENDED_H_

#include <mutex>
#include <atomic>
#include <iostream>

template<class T>
class MPMCDequeExtended{
public:
    MPMCDequeExtended(){
        Block *initial = new Block();
        initial->leftElement = initial->data + initial->kBlockSize / 2;
        initial->rightElement = initial->leftElement - 1;

        lBlock = initial;
        rBlock = initial;

        lEnd = new std::mutex();
        rEnd = new std::mutex();
    }

    void push_back(const T &val){
        rEnd->lock();
        rBlock->lock();

        if(rBlock->rightElement == rBlock->data + rBlock->kBlockSize){
            if(rBlock->rightSibling == nullptr){
                rBlock->rightSibling = new Block();
                rBlock->rightSibling->leftElement = rBlock->rightSibling->data;
                rBlock->rightSibling->rightElement = rBlock->rightSibling->leftElement - 1;
                rBlock->rightSibling->leftSibling = rBlock;
            }
            auto oldBlock = rBlock;
            rBlock = rBlock->rightSibling;
            rBlock->lock();
            oldBlock->unlock();
            new(++rBlock->rightElement) T(val);
            rBlock->unlock();
            rEnd->unlock();
            return;    
        }

        new(++rBlock->rightElement) T(val);

        rBlock->unlock();
        rEnd->unlock();
    }

    void push_front(const T &val){
        lEnd->lock();
        lBlock->lock();

        if(lBlock->leftElement == lBlock->data){
            if(lBlock->leftSibling == nullptr){
                lBlock->leftSibling = new Block();
                lBlock->leftSibling->leftElement = lBlock->leftSibling->data + lBlock->kBlockSize;
                lBlock->leftSibling->rightElement = lBlock->leftSibling->leftElement - 1;
                lBlock->leftSibling->rightSibling = lBlock;
            }
            auto oldBlock = lBlock;
            lBlock = lBlock->leftSibling;
            lBlock->lock();
            oldBlock->unlock();
            new(--lBlock->leftElement) T(val);
            lBlock->unlock();
            lEnd->unlock();
            return;
        }

        new(--lBlock->leftElement) T(val);

        lBlock->unlock();
        lEnd->unlock();
    }

    T pop_back(){
        if(rBlock->leftElement > rBlock->rightElement)
            rBlock = rBlock->leftSibling;
        
        T res(*rBlock->rightElement);

        rBlock->rightElement->~T();
        --rBlock->rightElement;

        return res;
    }

    ~MPMCDequeExtended(){
        Block
            *leftMostBlock = lBlock,
            *rightMostBlock = rBlock;

        while(leftMostBlock->leftSibling != nullptr)
            leftMostBlock = leftMostBlock->leftSibling;
        while(rightMostBlock->rightSibling != nullptr)
            rightMostBlock = rightMostBlock->rightSibling;
        
        while(leftMostBlock != rightMostBlock){
            leftMostBlock = leftMostBlock->rightSibling;
            delete leftMostBlock->leftSibling;
        }
        delete rightMostBlock;

        delete lEnd;
        delete rEnd;
    }

private:
    class Block{
    public:
        static constexpr size_t kBlockSize{100};

        Block(){
            data = static_cast<T*>(::operator new(sizeof(T) * kBlockSize, std::nothrow));
            if(data == nullptr)
                throw std::overflow_error("Cannot allocate memory");
        }

        T *data{nullptr};
        T *leftElement{nullptr}, *rightElement{nullptr};
        Block *leftSibling{nullptr}, *rightSibling{nullptr};

        inline void lock(){
            mux.lock();
        }
        inline void unlock(){
            mux.unlock();
        }

        ~Block(){
            while(leftElement <= rightElement){
                leftElement->~T();
                ++leftElement;
            }

            ::operator delete(data);
        }
    private:
        std::mutex mux{};
    };

    std::mutex *lEnd, *rEnd;
    Block *lBlock{nullptr}, *rBlock{nullptr};
};

#endif