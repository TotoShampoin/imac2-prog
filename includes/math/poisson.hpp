#pragma once
#include "exponential.hpp"

namespace Random {

template <typename Type>
class Poisson {
public:
    Poisson(Type parameter)
        : _parameter(parameter)
        , _rng(parameter) { }

    Type operator()() const {
        Type T = 0;
        uint N = 0;
        while (T < 1) {
            T += _rng();
            N++;
        }
        return static_cast<Type>(N - 1);
    }

    Type parameter() const { return _parameter; }

private:
    Type _parameter;
    Exponential<Type> _rng;
};

} // namespace Random
