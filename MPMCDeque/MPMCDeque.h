#ifndef MPMCDEQUE_H_
#define MPMCDEQUE_H_

#include <mutex>
#include <deque>

template<class T>
class MPMCDeque{
public:
    void push_back(const T& elem){
        std::lock_guard<std::recursive_mutex> lck(mux);
        d.push_back(elem);
    }

    void push_front(const T& elem){
        std::lock_guard<std::recursive_mutex> lck(mux);
        d.push_front(elem);
    }

    T pop_front(){
        std::lock_guard<std::recursive_mutex> lck(mux);
        T res = d.front();
        d.pop_front();
        return res;
    }

    T pop_back(){
        std::lock_guard<std::recursive_mutex> lck(mux);
        T res = d.back();
        d.pop_back();
        return res;
    }
private:
    std::recursive_mutex mux;
    std::deque<T> d;
};

#endif