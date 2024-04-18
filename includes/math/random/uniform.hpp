#pragma once
#include "base_rng.hpp"
#include <random>

namespace Random {

template <typename Type>
class Uniform : BaseRng<Type> {
public:
    explicit Uniform(Type min = 0, Type max = 1)
        : _min(min)
        , _max(max) { }

    Type operator()() override {
        return _distribution(_generator) * (_max - _min) + _min;
    }

    Type min() const { return _min; }
    Type max() const { return _max; }
    void min(Type min) { _min = min; }
    void max(Type max) { _max = max; }

private:
    Type _min, _max;
    std::random_device _random;
    std::mt19937 _generator { _random() };
    std::uniform_real_distribution<Type> _distribution { 0, 1 };
};

} // namespace Random
