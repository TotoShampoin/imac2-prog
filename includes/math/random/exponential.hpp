#pragma once
#include "base_rng.hpp"
#include "uniform.hpp"
#include <cmath>

namespace Random {

template <typename Type>
class Exponential : BaseRng<Type> {
public:
    explicit Exponential(Type parameter)
        : _parameter(parameter) { }

    Type operator()() {
        return -std::log(static_cast<Type>(1) - _rng()) / _parameter;
    }

    Type parameter() const { return _parameter; }

private:
    Type _parameter;
    Uniform<Type> _rng;
};

} // namespace Random
