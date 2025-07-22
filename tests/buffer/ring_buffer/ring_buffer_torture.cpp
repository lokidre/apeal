/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#include <test_main.h>

#include <test_buffer.hpp>
#include <string>
#include <apeal/random.hpp>

#include <apeal/buffer/linked_ring_buffer.hpp>

template <class T>
void compareTestContainers(const apl::RingBuffer<T>& buffer, const std::vector<T>& vec) {
    auto intBufferIt = buffer.begin();
    auto intVectorIt = vec.begin();
    auto testCount = buffer.size();

    for (auto i = 0; i < testCount; ++i) {
        auto vecVal = *intVectorIt++;
        auto bufferVal = *intBufferIt++;

        if (vecVal != bufferVal) {
            int a = 0;
            throw std::runtime_error("Wrong value for vector");
        }
    }
}

struct TestObject {
    int intVal;
    double doubleVal;
    std::string stringVal;

    auto operator<=>(const TestObject&) const = default;

};

void body() {
    using namespace apl;

    Random rand{true, 1};





    std::vector<int> intVector;
    RingBuffer<int> intBuffer;

    int intTestCount = 64;
    int intTest1Count = 128;
    int intTest2Count = 64;

    for (int i = 0; i < intTestCount; ++i) {
        for (int j = 0; j < intTest1Count; ++j) {

            int value{};
            std::string item{};

            rand.range(-65536, 65536, &value);

            intVector.push_back(value);
            intBuffer.push(value);
        }
        for (int k = 0; k < intTest2Count; ++k) {
            intVector.erase(intVector.begin());
            intBuffer.pop();
        }
    }






    std::vector<double> doubleVector;
    RingBuffer<double> doubleBuffer;

    int doubleTestCount = 64;
    int doubleTest1Count = 128;
    int doubleTest2Count = 64;

    for (int i = 0; i < doubleTestCount; ++i) {
        for (int j = 0; j < doubleTest1Count; ++j) {

            double value{};

            rand.range(-65536, 65536, &value);

            doubleVector.push_back(value);
            doubleBuffer.push(value);
        }
        for (int k = 0; k < doubleTest2Count; ++k) {
            doubleVector.erase(doubleVector.begin());
            doubleBuffer.pop();
        }
    }









    std::vector<std::string> strVector;
    RingBuffer<std::string> strBuffer;


    int strTestCount = 64;
    int strTest1Count = 128;
    int strTest2Count = 64;

    for (int i = 0; i < strTestCount; ++i) {
        for (int j = 0; j < strTest1Count; ++j) {

            std::vector<char> letters{};
            for (char letter = 'a'; letter <= 'z'; ++letter) {
                letters.push_back(letter);
            }
            int value{};
            std::string item{};

            int maxRange = 26;
            rand.range(0, maxRange, &value);
            for (int k = 0; k < value; ++k) {
                item.push_back(letters[value]);
            }

            strVector.push_back(item);
            strBuffer.emplace(std::move(item));
        }
        for (int k = 0; k < strTest2Count; ++k) {
            strVector.erase(strVector.begin());
            strBuffer.pop();
        }
    }



    

    std::vector<TestObject> testObjectVector;
    RingBuffer<TestObject> testObjectBuffer;


    int objTestCount = 128;
    int objTest1Count = 128;
    int objTest2Count = 64;

    for (int i = 0; i < objTestCount; ++i) {
        for (int j = 0; j < objTest1Count; ++j) {
            TestObject testObject{};
            rand.range(-1000, 1000, &testObject.intVal);
            rand.range(-1000, 1000, &testObject.doubleVal);
            std::vector<char> letters{};
            for (char letter = 'a'; letter <= 'z'; ++letter) {
                letters.push_back(letter);
            }
            int value{};
            std::string item{};

            int maxRange = 26;
            rand.range(0, maxRange, &value);
            for (int k = 0; k < value; ++k) {
                item.push_back(letters[value]);
            }
            testObject.stringVal = item;


            testObjectVector.push_back(testObject);
            testObjectBuffer.emplace(std::move(testObject));
        }
        for (int k = 0; k < objTest2Count; ++k) {
            testObjectVector.erase(testObjectVector.begin());
            testObjectBuffer.pop();
        }
    }

//
//    std::vector<TestObject> testBigVector;
//    LinkedRingBuffer<TestObject> testBigBuffer;
//
//
//    for(int p = 0; p<5; ++p) {
//
//        std::vector<TestObject> testOVector;
//        LinkedRingBuffer<TestObject> testOBuffer;
//
//        int objTestCount = 64;
//        int objTest1Count = 128;
//        int objTest2Count = 64;
//
//        for (int i = 0; i < objTestCount; ++i) {
//            for (int j = 0; j < objTest1Count; ++j) {
//                TestObject testObject{};
//                rand.range(-1000, 1000, &testObject.intVal);
//                rand.range(-1000, 1000, &testObject.doubleVal);
//                std::vector<char> letters{};
//                for (char letter = 'a'; letter <= 'z'; ++letter) {
//                    letters.push_back(letter);
//                }
//                int value{};
//                std::string item{};
//
//                int maxRange = 26;
//                rand.range(0, maxRange, &value);
//                for (int k = 0; k < value; ++k) {
//                    item.push_back(letters[value]);
//                }
//                testObject.stringVal = item;
//
//
//                testObjectVector.push_back(testObject);
//                testObjectBuffer.emplace(std::move(testObject));
//            }
//            for (int k = 0; k < objTest2Count; ++k) {
//                testObjectVector.erase(testObjectVector.begin());
//                testObjectBuffer.pop();
//            }
//        }
//        testBigVector.push_back(testOVector);
//        testBigBuffer.emplace(std::move(testOBuffer));
//
//    }




    compareTestContainers(intBuffer, intVector);
    compareTestContainers(doubleBuffer, doubleVector);
    compareTestContainers(strBuffer, strVector);
    compareTestContainers(testObjectBuffer, testObjectVector);


    int a = 0;
}
