#ifndef BLOCKCONTROLLER_H_
#define BLOCKCONTROLLER_H_

#include "Block.h"

#include <mutex>
#include <memory>

template<class T>
class BlockController{
public:
    BlockController(std::shared_ptr<Block<T>> block):
        block(block)
    {}

    void lock(std::shared_ptr<Block<T>> &prevvBlock){
        mux.lock();
        block->lock();
        prevvBlock = block;
    }
    void unlock(std::shared_ptr<Block<T>> &prevvBlock){
        prevvBlock->unlock();
        mux.unlock();
    }

    void push_back(const T &elem){
        std::shared_ptr<Block<T>> prevvBlock{nullptr};
        lock(prevvBlock);

        if(block->fullRight())
            growRight();

        ++block->rightElement;
        new(block->rightElement) T(elem);

        unlock(prevvBlock);
    }
    void push_front(const T &elem){
        std::shared_ptr<Block<T>> prevvBlock{nullptr};
        lock(prevvBlock);

        if(block->fullLeft())
            growLeft();
        
        --block->leftElement;
        new(block->leftElement) T(elem);

        unlock(prevvBlock);
    }

    T pop_front(){
        std::shared_ptr<Block<T>> prevvBlock{nullptr};
        lock(prevvBlock);

        if(block->empty())
            block = block->rightBlock;
        
        T res(*block->leftElement);

        block->leftElement->~T();
        ++block->leftElement;

        unlock(prevvBlock);

        return res;
    }
    T pop_back(){
        std::shared_ptr<Block<T>> prevvBlock{nullptr};
        lock(prevvBlock);

        if(block->empty())
            block = block->leftBlock;
        
        T res(*block->rightElement);

        block->rightElement->~T();
        --block->rightElement;

        unlock(prevvBlock);

        return res;
    }

    ~BlockController(){
        auto l = block, r = block;

        while(l->leftBlock != nullptr)
            l = l->leftBlock;
        while(r->rightBlock != nullptr)
            r = r->rightBlock;
        
        while(l != r){
            auto nextt = l->rightBlock;
            
            nextt->leftBlock = nullptr;
            l->rightBlock = nullptr;

            l = nextt;
        }
    }
private:
    std::mutex mux{};
    std::shared_ptr<Block<T>> block{nullptr};

    void growLeft(){
        if(block->leftBlock == nullptr){
            block->leftBlock = std::shared_ptr<Block<T>>(new Block<T>(BlockPolicy::left));
            block->leftBlock->rightBlock = block;
        }
    
        block = block->leftBlock;
    }
    void growRight(){
        if(block->rightBlock == nullptr){
            block->rightBlock = std::shared_ptr<Block<T>>(new Block<T>(BlockPolicy::right));
            block->rightBlock->leftBlock = block;
        }
        block = block->rightBlock;
    }
};

#endif