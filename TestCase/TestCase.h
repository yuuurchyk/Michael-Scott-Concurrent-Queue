#ifndef TESTCASE_H_
#define TESTCASE_H_

#include <string>
#include <list>
#include <stdexcept>

#ifndef TESTEXTENDED

#include "../MPMCDeque/MPMCDeque.h"
template<class T>
using testDeque = MPMCDeque<T>;

#else

// INLCUDE EXTENDED VERSION

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
    std::string description() const{
        return "Testing creation of deque and adding some elements in 1 thread";
    } 

    void run(){
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

    ~Creation() = default;
};

std::list<TestCase*> getTestCases();

#endif