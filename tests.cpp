#include <cstdlib>
#include <thread>
#include <mutex>
#include <vector>
#include <functional>
#include <algorithm>

#include <gtest/gtest.h>

#include "ConcurrentQueueBase/ConcurrentQueueBase.hpp"
#include "ConcurrentQueueSimple/ConcurrentQueueSimple.hpp"

typedef int T;

namespace{
    constexpr size_t kIncreaseToThousandN = 1000;

    std::vector<T> increaseToThousandInit(){
        static constexpr size_t kMaxInitial = 10;

        std::vector<T> res;

        for(size_t i = 0; i < kIncreaseToThousandN; ++i)
            res.push_back(rand() % kMaxInitial);
        
        return res;
    }

    const std::vector<T> increaseToThousandInitial = increaseToThousandInit();

    void increaseToThousandWorker(
        ConcurrentQueueBase<T> &from,
        ConcurrentQueueBase<T> &to
    ){
        T it = 0;

        while(from.tryPop(&it)){
            ++it;
            if(it == 1000)
                to.push(it);
            else
                from.push(it);
        }
    }
}

namespace{
    constexpr T kAddNumbersDeathPill = -1;
    constexpr size_t kAddNumbersN = 1000000;

    std::vector<T> addNumbersInit(){
        static constexpr size_t kMaxInitial = 10;

        std::vector<T> res;

        for(size_t i = 0; i < kAddNumbersN; ++i)
            res.push_back(rand() % kMaxInitial);
        
        return res;
    }

    const std::vector<T> addNumbersInitial = addNumbersInit();

    T addNumbersComputeSumm(){
        T res = 0;

        for(const auto &it: addNumbersInitial)
            res += it;
        
        return res;
    }

    T addNumbersSumm = addNumbersComputeSumm();

    void addNumbersProducer(
        size_t producersNum,
        size_t remainderr,
        ConcurrentQueueBase<T> &target
    )
    {
        for(size_t i = remainderr; i < addNumbersInitial.size(); i += producersNum)
            target.push(addNumbersInitial[i]);
    }

    void addNumbersConsumer(ConcurrentQueueBase<T> &target){
        T first = 0, second = 0, third = 0;

        std::unique_lock<std::recursive_mutex> lck(target.getPopMutex());
        while(true){
            if(!target.tryPop(&first)){
                // rule 1
                target.getCondVar().wait(lck);
                continue;
            }

            if(!target.tryPop(&second)){
                lck.unlock();

                // rule 2
                if(first == kAddNumbersDeathPill)
                    return;
                
                // rule 3
                target.push(first);
                lck.lock();
                continue;
            }

            if(!target.tryPop(&third)){
                lck.unlock();

                // rule 4
                if(
                    first == kAddNumbersDeathPill ||
                    second == kAddNumbersDeathPill
                )
                {
                    target.push(first);
                    target.push(second);
                    return;
                }

                // rule 5
                target.push(first + second);
                lck.lock();
                continue;
            }

            lck.unlock();

            // make death pill be in third number
            if(first == kAddNumbersDeathPill)
                std::swap(first, third);
            if(second == kAddNumbersDeathPill)
                std::swap(second, third);
            
            // rule 6
            if(third != kAddNumbersDeathPill){
                target.push(first + second + third);
                lck.lock();
                continue;
            }

            // rule 7
            target.push(first + second);
            target.push(third);
            lck.lock();
            continue;
        }
    }
}

template class ConcurrentQueueSimple<int>;
template class ConcurrentQueueSimple<std::vector<size_t>>;

template <typename T>
class ConcurrentQueueTest: public ::testing::Test { };
TYPED_TEST_CASE_P(ConcurrentQueueTest);

TYPED_TEST_P(ConcurrentQueueTest, Init){
    // Check whether initialization is ok

    TypeParam q;
}
TYPED_TEST_P(ConcurrentQueueTest, Description){
    // Check if description is ok

    TypeParam q;
    ASSERT_FALSE(q.description().empty());
}
TYPED_TEST_P(ConcurrentQueueTest, PushAndSizeAndEmptyOneThread){
    // Check whether push works fine and empty() and size() invariant are preserved
    // sequentially

    TypeParam q;
    ASSERT_TRUE(q.empty());
    ASSERT_EQ(q.size(), 0);

    q.push(1);

    ASSERT_FALSE(q.empty());
    ASSERT_EQ(q.size(), 1);

    for(T i = 2; i <= 1000000; ++i){
        q.push(i);
        ASSERT_FALSE(q.empty());
        ASSERT_EQ(q.size(), i);
    }
}
TYPED_TEST_P(ConcurrentQueueTest, PopAndTryPopOneThread){
    // Check whether pop and tryPop works fine sequentially

    TypeParam q;

    q.push(1);
    q.push(2);

    ASSERT_EQ(q.size(), 2);

    ASSERT_EQ(q.pop(), 1);
    ASSERT_EQ(q.size(), 1);
    ASSERT_FALSE(q.empty());

    ASSERT_EQ(q.pop(), 2);
    ASSERT_EQ(q.size(), 0);
    ASSERT_TRUE(q.empty());

    ASSERT_FALSE(q.tryPop());

    {
        std::lock_guard<TypeParam> lck(q);
        q.push(10);
        q.push(100);
    }

    T target = 0;
    ASSERT_EQ(target, 0);
    ASSERT_TRUE(q.tryPop(&target));
    ASSERT_EQ(target, 10);
}
TYPED_TEST_P(ConcurrentQueueTest, Front){
    // Check whether front is working fine sequentially

    TypeParam q;

    q.push(1);
    q.push(2);

    ASSERT_EQ(q.front(), 1);
    q.pop();
    ASSERT_EQ(q.front(), 2);
}
TYPED_TEST_P(ConcurrentQueueTest, IncreaseToThousand){
    // There are numbers in increaseToThousandInitial withing small range ([0; 10])
    // We push them in the queue and perform the following in multiple threads:
    //      get element from queue and increase in by 1
    //      if it is >= 1000, push it in result queue
    //      else push it back in the initial queue
    // We are using std::thread::hardware_concurrency() number of threads

    TypeParam qInitial, qTarget;

    for(T it: increaseToThousandInitial)
        qInitial.push(it);
    
    ASSERT_EQ(qInitial.size(), kIncreaseToThousandN);
    
    std::vector<std::thread> threads;
    for(size_t i = 0; i < std::thread::hardware_concurrency(); ++i)
        threads.push_back(std::thread(increaseToThousandWorker, std::ref(qInitial), std::ref(qTarget)));
    
    for(auto &it: threads)
        it.join();
    
    ASSERT_TRUE(qInitial.empty());
    ASSERT_EQ(qTarget.size(), kIncreaseToThousandN);

    for(size_t i = 0; i < kIncreaseToThousandN; ++i)
        ASSERT_EQ(qTarget.pop(), 1000);
    
    ASSERT_TRUE(qTarget.empty());
}
TYPED_TEST_P(ConcurrentQueueTest, AddNumbers){
    // Implementing MPMC pattern to compare implementations
    // of concurrent queue
    // There are producers: they push positive numbers in the queue
    // There are consumers: they pop 2 elements from the queue and push their sum back
    // The goal of the following process is to compute sum of all the numbers produced
    // Consumers and producers work in paralel, so -1 is considered as a death pill
    // Because we are using queue, not deque, I suggest the following algorithm for consumers
    // We'll try to pop 3 elements from the queue
    // Possible situations:
    //  1) Nothing => we should wait
    //  2) Death pill => consumer should die
    //  3) Number => consumer should push it back
    //  4) Number and death pill => consumer should push number and death pill back and die
    //  5) 2 numbers => consumer should add them, push sum back
    //  6) 3 numbers => same as in 5)
    //  7) 2 Numbers and death pill => add numbers, push sum back, push death pill back
    // We will be using half of available threads as producers and other half as consumers

    size_t
        producersNum = std::max(
            std::thread::hardware_concurrency() / 2,
            static_cast<unsigned int>(1)
        ),
        consumersNum = std::max(
            std::thread::hardware_concurrency() / 2,
            static_cast<unsigned int>(1)
        );
    
    TypeParam q;

    std::vector<std::thread>
        producerThreads,
        consumerThreads;
    
    for(size_t i = 0; i < producersNum; ++i)
        producerThreads.push_back(
            std::thread(
                addNumbersProducer,
                producersNum,
                i,
                std::ref(q)
            )
        );
    
    for(size_t j = 0; j < consumersNum; ++j)
        consumerThreads.push_back(
            std::thread(
                addNumbersConsumer,
                std::ref(q)
            )
        );
    
    for(auto &it: producerThreads)
        it.join();
    
    q.push(kAddNumbersDeathPill);

    for(auto &it: consumerThreads)
        it.join();
    
    ASSERT_EQ(q.size(), 2);

    T
        first = q.pop(),
        second = q.pop();
    
    if(first == kAddNumbersDeathPill)
        std::swap(first, second);
    
    ASSERT_EQ(first, addNumbersSumm);
    ASSERT_EQ(second, kAddNumbersDeathPill);
}

REGISTER_TYPED_TEST_CASE_P(
    ConcurrentQueueTest,
    Init,
    Description,
    PushAndSizeAndEmptyOneThread,
    PopAndTryPopOneThread,
    Front,
    IncreaseToThousand,
    AddNumbers
);

typedef ::testing::Types<ConcurrentQueueSimple<T>> ConcurrentQueueTypes;
INSTANTIATE_TYPED_TEST_CASE_P(ConcurrentQueueInstantiation, ConcurrentQueueTest, ConcurrentQueueTypes);

int main(int argc, char **argv){
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}