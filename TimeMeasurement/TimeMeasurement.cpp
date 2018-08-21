#include "TimeMeasurement.h"

#include <ctype.h>
#include <climits>
#include <algorithm>

double timeIt(TestCase &testCase){
    long long bestTime = LLONG_MAX;

    for(size_t i = 0; i < kRepetitionsNum; ++i){
        auto start = get_current_time_fenced();
        testCase.run();
        auto end = get_current_time_fenced();

        bestTime = std::min(bestTime, to_us(end - start));
    }

    return static_cast<double>(bestTime) / 1000000;
}
