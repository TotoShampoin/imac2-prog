#pragma once
#include "uniform.hpp"
#include <cmath>

namespace Random {

template <typename Type>
class Exponential {
public:
    Exponential(Type parameter)
        : parameter(parameter) { }

    Type operator()() {
        return -std::log(static_cast<Type>(1) - rng()) / parameter;
    }

private:
    Type parameter;
    Uniform<Type> rng;
};

} // namespace Random
