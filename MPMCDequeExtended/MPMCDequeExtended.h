#ifndef MPMCDEQUEEXTENDED_H_
#define MPMCDEQUEEXTENDED_H_

#include "Block.h"
#include "BlockController.h"

#include <memory>

template<class T>
class MPMCDequeExtended{
public:
    MPMCDequeExtended(){
        std::shared_ptr<Block<T>> block(new Block<T>(BlockPolicy::middle));

        l = std::unique_ptr<BlockController<T>>(new BlockController<T>(block));
        r = std::unique_ptr<BlockController<T>>(new BlockController<T>(block));
    }

    void push_back(const T &elem){
        return r->push_back(elem);
    }
    void push_front(const T &elem){
        return l->push_front(elem);
    }

    T pop_front(){
        return l->pop_front();
    }
    T pop_back(){
        return r->pop_back();
    }

    ~MPMCDequeExtended() = default;
private:
    std::unique_ptr<BlockController<T>> l{nullptr}, r{nullptr};
};

#endif