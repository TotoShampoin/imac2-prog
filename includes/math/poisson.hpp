#pragma once
#include "exponential.hpp"

namespace Random {

template <typename Type, typename RandomType = float>
class Poisson {
public:
    Poisson(RandomType parameter)
        : _parameter(parameter)
        , _rng(parameter) { }

    Type operator()() {
        RandomType T = 0;
        Type N = 0;
        while (T < 1) {
            T += _rng();
            N++;
        }
        return static_cast<Type>(N - 1);
    }

    RandomType parameter() const { return _parameter; }

private:
    RandomType _parameter;
    Exponential<RandomType> _rng;
};

} // namespace Random
