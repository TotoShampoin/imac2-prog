#pragma once
#include "uniform.hpp"
#include "utils.hpp"
#include <stdexcept>
#include <vector>

namespace Random {

template <typename Type>
class Binomial {
public:
    Binomial(Type probability, int trials)
        : _probability(probability)
        , _trials(trials) {
        _probability_list.reserve(trials + 1);
        for (int i = 0; i <= trials; i++) {
            _probability_list.push_back(
                Math::binomialProbability<Type>(probability, trials, i));
        }
    }

    Type operator()() const {
        Type x = _rng();
        Type sum = 0;
        for (int i = 0; i < _probability_list.size(); i++) {
            sum += _probability_list[i];
            if (x < sum) {
                return i;
            }
        }
        throw std::runtime_error("Binomial distribution failed");
    }

    Type probability() const { return _probability; }
    int trials() const { return _trials; }

private:
    Type _probability;
    int _trials;
    Uniform<Type> _rng;
    std::vector<Type> _probability_list;
};

} // namespace Random
