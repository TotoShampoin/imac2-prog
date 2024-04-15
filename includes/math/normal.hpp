#pragma once

#include "math/base_rng.hpp"
#include "uniform.hpp"

namespace Random {

// Box-Muller transform
// https://en.wikipedia.org/wiki/Box%E2%80%93Muller_transform
template <typename Type>
class Normal : BaseRng<Type> {
public:
    Type operator()() {
        Type u1 = _rng_1();
        Type u2 = _rng_2();
        return sqrt(-2 * log(u1)) * cos(2 * M_PI * u2);
    }

    std::pair<Type, Type> generatePair() {
        Type u1 = _rng_1();
        Type u2 = _rng_2();
        Type r = sqrt(-2 * log(u1));
        return { r * cos(2 * M_PI * u2), r * sin(2 * M_PI * u2) };
    }

private:
    Uniform<Type> _rng_1 { 0, 1 };
    Uniform<Type> _rng_2 { 0, 1 };
};

} // namespace Random
