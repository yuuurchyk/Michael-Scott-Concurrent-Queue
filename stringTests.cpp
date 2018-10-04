#include <string>
#include <set>
#include <thread>
#include <vector>
#include <functional>
#include <iostream>

#include <gtest/gtest.h>

#include "ConcurrentQueueBase/ConcurrentQueueBase.hpp"
#include "ConcurrentQueueSimple/ConcurrentQueueSimple.hpp"
#include "ConcurrentQueueExtended/ConcurrentQueueExtended.hpp"

template <typename T>
class ConcurrentQueueTest: public ::testing::Test { };
TYPED_TEST_CASE_P(ConcurrentQueueTest);

namespace{
    const std::string kSampleText{"Loremipsumdolorsitamet,consecteturadipiscingelit.Utfelisante,portainullamcorperid,vehiculavelpurus.Prointristiqueconsecteturarcuutvenenatis.Etiammetusdiam,dictumsedmetuseget,consequatfinibuslorem.Inhachabitasseplateadictumst.Crasacmauriseuarcususcipiteuismod.Nullacursusliberonondapibuspellentesque.Suspendissepotenti.Aliquamvestibulumluctusdolor.Vestibulumnunclectus,rutrumutipsumac,tristiqueauctorneque.Maurisquisbibendumeros.Aliquameratvolutpat.Donecaliquamnisietligulahendrerit,idaccumsannuncfermentum.Suspendissepotenti.Proinaccumsannibhutnuncluctuseuismod.Utsitamettincidunttellus,sedfermentumurna."};
    const std::string kPill{""};
    size_t kSamplesNum{0};
    const size_t kProducersNum{
        std::max(
            std::thread::hardware_concurrency() / 2,
            static_cast<unsigned int>(1)
        )
    };

    bool check(const std::string &s){
        std::stringstream ss(s);

        std::set<size_t> pieces{};
        std::string current{};

        while(ss >> current){
            if(current != "Sample")
                return false;
            
            size_t currentPiece;
            if(!(ss >> currentPiece))
                return false;
            
            if(pieces.find(currentPiece) != pieces.end())
                return false;
            pieces.insert(currentPiece);

            if(!(ss >> current))
                return false;
            if(current != kSampleText)
                return false;
        }
        return pieces.size() == kSamplesNum;
    }

    std::string generate(size_t pieceNum){
        std::stringstream ss;
        ss << "Sample " << pieceNum << " " << kSampleText << " ";
        return ss.str();
    }

    void produce(size_t remainder, ConcurrentQueueBase<std::string> &target){
        for(size_t i = remainder; i < kSamplesNum; i += kProducersNum)
            target.push(generate(i));
    }

    void consume(ConcurrentQueueBase<std::string> &target){
        std::string first{""}, second{""}, third{""};

        std::unique_lock<std::recursive_mutex> lck(target.getPopMutex());
        lck.unlock();
        while(true){
            lck.lock();

            if(!target.tryPop(&first)){
                // rule 1
                lck.unlock();
                continue;
            }

            if(!target.tryPop(&second)){
                lck.unlock();

                // rule 2
                if(first == kPill)
                    return target.push(first);
                
                // rule 3
                target.push(first);
                continue;
            }

            if(!target.tryPop(&third)){
                lck.unlock();

                // rule 4
                if(
                    first == kPill ||
                    second == kPill
                )
                {
                    target.push(first);
                    target.push(second);
                    return;
                }

                // rule 5
                target.push(first + second);
                continue;
            }

            lck.unlock();

            // make death pill be in third number
            if(first == kPill)
                std::swap(first, third);
            if(second == kPill)
                std::swap(second, third);
            
            // rule 6
            if(third != kPill){
                target.push(first + second + third);
                continue;
            }

            // rule 7
            target.push(first + second);
            target.push(third);
            continue;
        }
    }
}

TYPED_TEST_P(ConcurrentQueueTest, Init){
    // Check whether initialization is ok
    TypeParam q;
}
TYPED_TEST_P(ConcurrentQueueTest, ConcatenationSequential){
    // Check whether push is ok sequentially

    TypeParam q{};
    
    for(size_t i = 0; i < kSamplesNum; ++i)
        q.push(generate(i));
    
    std::string res{""};
    for(size_t i = 0; i < kSamplesNum; ++i){
        std::string current{""};
        ASSERT_TRUE(q.tryPop(&current));
        res += current;
    }

    {
        std::string dummy{""};
        ASSERT_FALSE(q.tryPop(&dummy));
    }
    
    ASSERT_TRUE(check(res));
}
TYPED_TEST_P(ConcurrentQueueTest, ConcatenationMultiThreadProducer){
    // Check whether push is ok

    TypeParam q{};

    std::vector<std::thread> producers{};
    for(size_t i = 0; i < kProducersNum; ++i)
        producers.push_back(std::thread(produce, i, std::ref(q)));
    
    for(auto &producer: producers)
        producer.join();
    
    std::string res{""};
    std::string current{""};
    while(q.tryPop(&current))
        res += current;
    
    ASSERT_TRUE(check(res));
}
TYPED_TEST_P(ConcurrentQueueTest, ConcatenationMPMC){
    // Check whether push and pop work fine in parallel
    // Logic and rules are the same as in tests.cpp AddNumbers test

    TypeParam q{};

    size_t
        consumersNum = std::max(
            std::thread::hardware_concurrency() / 2,
            static_cast<unsigned int>(1)
        );
    
    std::vector<std::thread>
        consumerThreads{},
        producerThreads{};
    
    for(size_t i = 0; i < kProducersNum; ++i)
        producerThreads.push_back(std::thread(produce, i, std::ref(q)));
    
    for(size_t i = 0; i < consumersNum; ++i)
        consumerThreads.push_back(std::thread(consume, std::ref(q)));
    
    for(auto &it: producerThreads)
        it.join();
    
    q.push(kPill);

    for(auto &it: consumerThreads)
        it.join();
    
    std::string first{""}, second{""};
    ASSERT_TRUE(q.tryPop(&first));
    ASSERT_TRUE(q.tryPop(&second));
    
    {
        std::string dummy{""};
        ASSERT_FALSE(q.tryPop(&dummy));
    }

    if(first == kPill)
        std::swap(first, second);
    
    ASSERT_TRUE(check(first));
}

REGISTER_TYPED_TEST_CASE_P(
    ConcurrentQueueTest,
    Init,
    ConcatenationSequential,
    ConcatenationMultiThreadProducer,
    ConcatenationMPMC
);

typedef ::testing::Types<ConcurrentQueueSimple<std::string>, ConcurrentQueueExtended<std::string>> ConcurrentQueueTypes;
INSTANTIATE_TYPED_TEST_CASE_P(ConcurrentQueueInstantiation, ConcurrentQueueTest, ConcurrentQueueTypes);

int main(int argc, char **argv){
    {
        size_t defaultN = 100;

        std::stringstream ss{""};
        switch(argc){
            case 0: case 1:
                kSamplesNum = defaultN;
                break;
            case 2:
                ss.str(*(++argv));
                if(!(ss>>kSamplesNum))
                    throw std::runtime_error("Invalid argument");
                --argv;
                break;
            default:
                throw std::runtime_error("Invalid number of arguments");
                break;
        }
    }

    std::cout << "Testing:" << std::endl;
    std::cout << "\tSamples num: " << kSamplesNum << std::endl;

    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
