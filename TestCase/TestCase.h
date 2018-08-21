#ifndef TESTCASE_H_
#define TESTCASE_H_

#include <string>
#include <list>
#include <stdexcept>
#include <vector>
#include <thread>
#include <functional>
#include <algorithm>

#ifndef TESTEXTENDED

#include "../MPMCDeque/MPMCDeque.h"
template<class T>
using testDeque = MPMCDeque<T>;

#else

#include "../MPMCDequeExtended/MPMCDequeExtended.h"
template<class T>
using testDeque = MPMCDequeExtended<T>;

#endif

class TestCase{
public:
    virtual std::string description() const = 0;
    virtual void run() = 0;

    void fail(){
        throw std::logic_error("Invalid data");
    }

    virtual ~TestCase() = default;
};


class Creation: public TestCase{
public:
    virtual std::string description() const final{
        return "Testing creation of deque and adding some elements sequentially";
    } 

    virtual void run() final{
        testDeque<int> d;
        d.push_back(1);
        
        if(d.pop_front() != 1)
            fail();
        
        d.push_front(2);
        d.push_front(3);

        if(d.pop_back() != 2)
            fail();
        if(d.pop_back() != 3)
            fail();
    }

    virtual ~Creation() final = default;
};


class MultithreadPush: public TestCase{
public:
    virtual std::string description() const = 0;

    virtual void run() final{
        using std::vector;
        using std::thread;
        using std::ref;
        using std::sort;

        testDeque<size_t> d;

        vector<thread> threads;
        threads.push_back(thread(&MultithreadPush::oddPusher, this, ref(d)));
        threads.push_back(thread(&MultithreadPush::evenPusher, this, ref(d)));

        for(auto &it: threads)
            it.join();
        
        // vector<size_t> obtainedValues;
        // obtainedValues.reserve(kMaxN + 1);

        // for(size_t i = 0; i <= kMaxN; ++i)
        //     obtainedValues.push_back(d.pop_back());
        
        // sort(obtainedValues.begin(), obtainedValues.end());

        // for(size_t i = 0; i <= kMaxN; ++i)
        //     if(obtainedValues[i] != i)
        //         fail();
    }

    virtual ~MultithreadPush() = default;
protected:
    static constexpr size_t kMaxN = 10000000;

    virtual void oddPusher(testDeque<size_t> &target) const = 0;
    virtual void evenPusher(testDeque<size_t> &target) const = 0;
};


class MultithreadPushOneDirection: public MultithreadPush{
public:
    virtual std::string description() const final{
        return "Pushing elements in 2 threads through 1 end";
    }

    virtual ~MultithreadPushOneDirection() final = default;
protected:
    virtual void oddPusher(testDeque<size_t> &target) const final{
        for(size_t i = 1; i <= kMaxN; i+=2)
            target.push_back(i);
    }

    virtual void evenPusher(testDeque<size_t> &target) const final{
        for(size_t i = 0; i <= kMaxN; i+=2)
            target.push_back(i);    
    }
};


class MultithreadPushTwoDirections: public MultithreadPush{
public:
    virtual std::string description() const final{
        return "Pushing elements in 2 threads through 2 ends";
    }

    virtual ~MultithreadPushTwoDirections() final = default;
protected:
    virtual void oddPusher(testDeque<size_t> &target) const final{
        for(size_t i = 1; i <= kMaxN; i+=2)
            target.push_front(i);
    }

    virtual void evenPusher(testDeque<size_t> &target) const final{
        for(size_t i = 0; i <= kMaxN; i+=2)
            target.push_back(i);    
    }
};


std::list<TestCase*> getTestCases();

#endif