#pragma once
#include "base_rng.hpp"
#include "exponential.hpp"

namespace Random {

template <typename Type, typename RandomType = float>
class Poisson : BaseRng<Type> {
public:
    explicit Poisson(RandomType parameter)
        : _parameter(parameter)
        , _rng(parameter) { }

    Type operator()() override {
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
