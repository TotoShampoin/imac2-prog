#pragma once
#include "exponential.hpp"

namespace Random {

template <typename Type>
class Poisson {
public:
    Poisson(Type parameter)
        : rng(parameter) { }

    Type operator()() {
        Type T = 0;
        uint N = 0;
        while (T < 1) {
            T += rng();
            N++;
        }
        return static_cast<Type>(N - 1);
    }

private:
    Exponential<Type> rng;
};

} // namespace Random
