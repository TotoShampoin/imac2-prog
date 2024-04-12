#pragma once
#include "uniform.hpp"
#include "utils.hpp"
#include <stdexcept>
#include <vector>

namespace Random {

template <typename Type>
// template <typename Type, Type probability, int trials>
class Binomial {
public:
    Binomial(Type probability, int trials) {
        // Binomial() {
        probability_list.reserve(trials + 1);
        for (int i = 0; i <= trials; i++) {
            probability_list.push_back(
                Math::binomialProbability<Type>(probability, trials, i));
        }
    }

    Type operator()() {
        Type x = rng();
        Type sum = 0;
        for (int i = 0; i < probability_list.size(); i++) {
            sum += probability_list[i];
            if (x < sum) {
                return i;
            }
        }
        throw std::runtime_error("Binomial distribution failed");
    }

private:
    Uniform<Type> rng;
    std::vector<Type> probability_list;
};

} // namespace Random
