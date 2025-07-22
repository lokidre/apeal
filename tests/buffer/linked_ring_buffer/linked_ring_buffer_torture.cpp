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
void compareTestContainers(const apl::LinkedRingBuffer<T>& buffer, const std::vector<T>& vec) {
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

struct TestRingBufferObject {
    int intVal;
    double doubleVal;
    std::string stringVal;

    apl::LinkedRingBuffer<TestObject> ringBuf;


};


void testIntBuffer() {
    apl::Random rand{true, 1};

    std::vector<int> intVector;
    apl::LinkedRingBuffer<int> intBuffer;

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

    compareTestContainers(intBuffer, intVector);
}

void testDoubBuffer() {
    apl::Random rand{true, 1};

    std::vector<double> doubleVector;
    apl::LinkedRingBuffer<double> doubleBuffer;

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

    compareTestContainers(doubleBuffer, doubleVector);
}

void testStrBuffer() {

    apl::Random rand{true, 1};
    std::vector<std::string> strVector;
    apl::LinkedRingBuffer<std::string> strBuffer;


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
    compareTestContainers(strBuffer, strVector);
}

void testObjBuffer() {
    apl::Random rand{true, 1};
    std::vector<TestObject> testObjectVector;
    apl::LinkedRingBuffer<TestObject> testObjectBuffer;


    int objTestCount = 64;
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
    compareTestContainers(testObjectBuffer, testObjectVector);
}

void testBufBuffer() {
    apl::Random rand{true, 1};
    std::vector<TestRingBufferObject> testBigVector;
    apl::LinkedRingBuffer<TestRingBufferObject> testBigBuffer;


    int objTestCount = 64;
    int objTest1Count = 128;
    int objTest2Count = 64;

    for (int i = 0; i < objTestCount; ++i) {
        for (int j = 0; j < objTest1Count; ++j) {
            TestRingBufferObject testObject{};
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

            // testObject.ringBuf.push

            testBigVector.push_back(testObject);
            testBigBuffer.emplace(std::move(testObject));
        }
        for (int k = 0; k < objTest2Count; ++k) {
            testBigVector.erase(testBigVector.begin());
            testBigBuffer.pop();
        }
    }

    compareTestContainers(testBigBuffer, testBigVector);
}


void body() {
    using namespace apl;


    testIntBuffer();
    testDoubBuffer();
    testObjBuffer();
    testStrBuffer();
    testBufBuffer();


    /*
    auto intBufferIt = intBuffer.begin();
    auto intVectorIt = intVector.begin();

    for (int i = 0; i < testCount; ++i) {
        auto vecVal = *intVectorIt++;
        auto bufferVal = *intBufferIt++;

        if (vecVal != bufferVal) {
            int a = 0;
            throw std::runtime_error("Wrong value for vector");
        }
    }
    */


    // LinkedRingBuffer<double> doubleBuffer;
    // LinkedRingBuffer<String> stringBuffer;

    int a = 0;
}
