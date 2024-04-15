#pragma once

#include "math/base_rng.hpp"
#include "uniform.hpp"

namespace Random {

// Box-Muller transform
// https://en.wikipedia.org/wiki/Box%E2%80%93Muller_transform
template <typename Type>
class Normal : BaseRng<Type> {
public:
    explicit Normal(Type mean = 0, Type standard_deviation = 1)
        : _mean(mean)
        , _standard_deviation(standard_deviation) { }

    Type operator()() {
        Type u1 = _rng_1();
        Type u2 = _rng_2();
        return _standard_deviation * sqrt(-2 * log(u1)) * cos(2 * M_PI * u2) + _mean;
    }

    std::pair<Type, Type> generatePair() {
        Type u1 = _rng_1();
        Type u2 = _rng_2();
        Type r = _standard_deviation * sqrt(-2 * log(u1));
        return { r * cos(2 * M_PI * u2) + _mean, r * sin(2 * M_PI * u2) + _mean };
    }

private:
    Type _mean { 0 };
    Type _standard_deviation { 1 };
    Uniform<Type> _rng_1 { 0, 1 };
    Uniform<Type> _rng_2 { 0, 1 };
};

} // namespace Random
