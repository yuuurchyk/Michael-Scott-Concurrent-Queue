#include "TestCase.h"

std::list<TestCase*> getTestCases(){
    std::list<TestCase*> res;

    res.push_back(new Creation());

    return res;
}