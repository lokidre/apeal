/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include "types.hpp"
#include "hash/md5.hpp"

#include "stl.hpp"
#include "val_range.hpp"

#include <concepts>
//#include <cstdlib>
#include <limits>
#include <random>
//#include <type_traits>


namespace apl {

class RandomSeedGenerator {

    HashMD5 distiller_;

public:
    RandomSeedGenerator() noexcept {
        distiller_.init();
    }

    void generateHash(uint32_t* hash) noexcept {
        collectNoise();
        distiller_.final(hash);
    }

    std::seed_seq generateSeed() noexcept {
        uint32_t hash[4];
        generateHash(hash);

        return std::seed_seq{hash[0], hash[1], hash[2], hash[3]};
    }

    void collectNoise() noexcept;


private:
    void noise(const void* data, std::size_t length) noexcept {
        distiller_.update(data, length);
    }

    template <class T>
    void noise(T val) noexcept { noise(&val, sizeof(val)); }

};


template <class T = int>
class RandomUniformIntDistribution {
    using Self = RandomUniformIntDistribution;

public:
    using param_type = std::pair<T, T>;
    using result_type = T;

    static constexpr result_type min() noexcept { return std::numeric_limits<result_type>::min(); }
    static constexpr result_type max() noexcept { return std::numeric_limits<result_type>::max(); }


    RandomUniformIntDistribution() noexcept: param_{} {}

    RandomUniformIntDistribution(T a, T b) noexcept: param_{a,b} {}

    result_type a() const noexcept { return param_.first; }
    result_type b() const noexcept { return param_.second; }

    const param_type& param() const noexcept { return param_; }
    void param(const param_type& p) noexcept { param_ = p; }

    void reset() {}

    template <class Generator>
    result_type range(Generator& g, result_type a, result_type b) noexcept 
    {
        if (a >= b)
            return a;

        using UT = typename std::make_unsigned<T>::type;

        auto x = g();
        auto u = static_cast<UT>(x);
        u >>= 1;  // Make it positive

        return result_type(u) % (b - a + 1) + a;
    }

    template <class Generator>
    result_type operator()(Generator& g) noexcept { 
        return range(g, param_.first, param_.second); 
    }

    template <class Generator>
    result_type operator()(Generator& g, const param_type& p) noexcept { 
        return range(g, p.first, p.second); 
    }

private:
    param_type param_;
};


template <class Generator, bool PseudoRandom = false>
class RandomBase {

public:
    // UniformRandomBitGenerator 
    using generator_type = Generator;
    using result_type = uint64_t;

    static constexpr result_type min() noexcept { return Generator::min(); }
    static constexpr result_type max() noexcept { return Generator::max(); }


    using UniformInt = RandomUniformIntDistribution<int>;

    using UniformIntParamType = UniformInt::param_type;


public:
    bool pseudoRandomOnly{PseudoRandom};  // use only pseudo random values
    bool pseudoRandomNever{false};  // never use pseudo random numbers
    int seedRequestFrequency{300};  // how many times use pseudo random number generator

    int retryDuplicates{1};  // 1 - don't retry


public:
    RandomBase() noexcept  {}

    RandomBase(bool pr, Index seed) noexcept {
        setPseudoRandom(pr, seed);
    }

    void setPseudoRandom(bool pr, Index seed) noexcept {
        pseudoRandomOnly = pr;
        randGenerator_.seed(seed);
    }


private:
    int lastValue_ = -1;
    int currentPseudoRandomCount_ = 0;

    Generator randGenerator_;
    RandomSeedGenerator seedGenerator_;


public:
    void seed(result_type s) noexcept {
        randGenerator_.seed(s);
    }

    template <class Sseq>
    void seed(const Sseq& seq) noexcept {
        randGenerator_.seed(seq);
    }

    // Generate random numbers
    result_type generate() noexcept {
        if (currentPseudoRandomCount_ <= 0 && !pseudoRandomOnly) {
            // generate as many random bytes as we need
            auto s = seedGenerator_.generateSeed();
            randGenerator_.seed(s);
            currentPseudoRandomCount_ = seedRequestFrequency;
        }

        currentPseudoRandomCount_--;

        return randGenerator_();
    }


    // Produces random number in [0, b] range
    result_type generate(result_type b) noexcept {
        if (b < 1)
            return 0;
        auto val = generate();
        return val % (b + 1);
    }

    // Returns real value in range [a, b)
    template <class T, class A, class B>
    T generateReal(A ai, B bi) noexcept {
        auto a = static_cast<T>(ai);
        auto b = static_cast<T>(bi);
        std::uniform_real_distribution<T> dis(a, b);
        return dis(*this);
    }


    // produces random number in [a, b] range
    Index generateRange(Index a, Index b) noexcept {
        if (a == b)
            return a;

        if (a > b)
            std::swap(a, b);

        auto num = std::abs((Index)generate());
        auto val = num % (b - a + 1) + a;

        return val;
    }

    Index index(result_type range) noexcept {  return (Index)generate(range - 1);  }
    int indexInt(result_type range) noexcept {  return (int)generate(range - 1);  }

    template <typename T>
    T index(result_type range, T* res) noexcept { 
        *res = static_cast<T>(index(range)); 
        return *res;
    }

    template <std::integral T, typename A, typename B>
    T range(A start, B bound, T* res) noexcept {
        auto a = static_cast<T>(start);
        auto b = static_cast<T>(bound);
        auto r = generateRange(a, b);
        *res = static_cast<T>(r);
        return *res;
    }


    template <std::floating_point T, typename A, typename B>
    T range(A a, B b, T* res) noexcept {
        *res = generateReal<T>(a, b);
        return *res;
    }

    int rangeInt(int start, int bound) noexcept {
        int val{};
        range(start, bound, &val);
        return val;
    }

    template <class T, class U>
    U range(const ValRange<T>& rng, U* res) noexcept {
        return range(rng.mn, rng.mx, res);
    }


    template <class T>
    T range(const ValRange<T>& rng) noexcept {
        T res;
        return range(rng, &res);
    }



    template <typename T, typename ValT, typename MinT, typename MaxT>
    T variation(ValT valT, MinT mnT, MaxT mxT, T* res) noexcept {
        auto val = static_cast<T>(valT);
        auto mn = static_cast<T>(mnT);
        auto mx = static_cast<T>(mxT);

        return range(val + mn, val + mx, res);
    }


    auto operator()() noexcept { return generate(); }
};


class Random: public RandomBase<std::mt19937_64> {
public:
    Random() noexcept {}
    Random(bool pseudo, Index seed) noexcept: RandomBase{pseudo, seed} {}
};

}  // namespace apl

