#pragma once
#include "uniform.hpp"
#include "utils.hpp"
#include <vector>
// #include <stdexcept>

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

    Type operator()() {
        Type x = _rng();
        Type sum = 0;
        for (int i = 0; i < _probability_list.size(); i++) {
            sum += _probability_list[i];
            if (x <= sum) {
                return i;
            }
        }
        return _trials; // should never happen, but it does because of floating point errors
        // throw std::runtime_error("Binomial distribution failed");
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
