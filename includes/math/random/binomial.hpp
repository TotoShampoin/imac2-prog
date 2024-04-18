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
        recalculateDistribution();
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

    void probability(RandomType probability) {
        _probability = probability;
        recalculateDistribution();
    }
    void trials(Type trials) {
        _trials = trials;
        recalculateDistribution();
    }

private:
    RandomType _probability;
    Type _trials;
    Uniform<RandomType> _rng;
    std::vector<RandomType> _probability_list;

    void recalculateDistribution() {
        _probability_list.clear();
        _probability_list.reserve(_trials + 1);
        for (int i = 0; i <= _trials; i++) {
            _probability_list.push_back(
                Math::binomialProbability<RandomType>(_probability, _trials, i));
        }
    }
};

} // namespace Random