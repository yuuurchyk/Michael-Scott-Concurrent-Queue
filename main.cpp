#include <iostream>
#include <iomanip>

#include "TestCase/TestCase.h"
#include "TimeMeasurement/TimeMeasurement.h"

int main(){
    using std::cout;
    using std::endl;
    using std::fixed;
    using std::setprecision;

    auto testCases = getTestCases();

    #ifndef TESTEXTENDED
        cout << "Testing simple concurrent deque" << endl;
    #else
        cout << "Testing extended concurrent deque" << endl;
    #endif

    for(auto it: testCases){
        if(it == nullptr)
            continue;

        cout << "----------" << endl;
        cout << it->description() << endl;

        cout << "Best time: ";
        cout << fixed << setprecision(12) << timeIt(*it);
        cout << "s" << endl;

        cout << "----------" << endl;
    }

    for(auto it: testCases)
        delete it;
    
    return 0;
}
