#pragma once
#include <random>

namespace Random {

template <typename Type>
class Uniform {
public:
    Type operator()() const {
        return distribution(generator);
    }

private:
    std::random_device random;
    std::mt19937 generator { random() };
    std::uniform_real_distribution<Type> distribution;
};

} // namespace Random
