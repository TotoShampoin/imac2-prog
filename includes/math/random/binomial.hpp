#pragma once
#include "base_rng.hpp"
#include "uniform.hpp"
#include "utils.hpp"
#include <vector>
// #include <stdexcept>

namespace Random {

template <typename Type, typename RandomType = float>
class Binomial : BaseRng<Type> {
public:
    Binomial(RandomType probability, Type trials)
        : _probability(probability)
        , _trials(trials) {
        _probability_list.reserve(trials + 1);
        for (int i = 0; i <= trials; i++) {
            _probability_list.push_back(
                Math::binomialProbability<RandomType>(probability, trials, i));
        }
    }

    Type operator()() {
        RandomType x = _rng();
        RandomType sum = 0;
        for (Type i = 0; i < _probability_list.size(); i++) {
            sum += _probability_list[i];
            if (x <= sum) {
                return i;
            }
        }
        return _trials; // should never happen, but it does because of floating point errors
        // throw std::runtime_error("Binomial distribution failed");
    }

    RandomType probability() const { return _probability; }
    Type trials() const { return _trials; }

private:
    RandomType _probability;
    Type _trials;
    Uniform<RandomType> _rng;
    std::vector<RandomType> _probability_list;
};

} // namespace Random
