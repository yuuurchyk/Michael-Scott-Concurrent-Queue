#include "TestCase.h"

std::list<TestCase*> getTestCases(){
    std::list<TestCase*> res;

    res.push_back(new Creation());
    res.push_back(new MultithreadPushOneDirection());
    res.push_back(new MultithreadPushTwoDirections());

    return res;
}