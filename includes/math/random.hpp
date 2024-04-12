#pragma once
#include <cmath>
#include <random>
#include <stdexcept>

namespace Random {

namespace {

constexpr uint64_t factorial(uint8_t n) {
    uint64_t result = 1;
    for (uint8_t i = 1; i <= n; i++) {
        result *= i;
    }
    return result;
}
constexpr uint64_t binom(uint8_t n, uint8_t k) {
    return factorial(n) / (factorial(k) * factorial(n - k));
}

template <typename Type>
constexpr Type binomialProbability(Type p, uint8_t n, uint8_t k) {
    return binom(n, k) * std::pow(p, k) * std::pow(1 - p, n - k);
}

} // namespace

template <typename Type>
Type uniform() {
    static auto random = std::random_device();
    static auto generator = std::mt19937(random());
    static auto distribution = std::uniform_real_distribution<Type>(0, 1);
    return distribution(generator);
}

template <typename Type>
Type exponential(Type parameter) {
    return -std::log(static_cast<Type>(1) - uniform<Type>()) / parameter;
}

template <typename Type>
Type poisson(Type parameter) {
    Type T = 0;
    uint N = 0;
    while (T < 1) {
        T += exponential<Type>(parameter);
        N++;
    }
    return static_cast<Type>(N - 1);
}

template <typename Type>
Type binomial(Type p, int n) {
    std::vector<Type> probabilities(n + 1);
    for (uint8_t i = 0; i <= n; i++) {
        probabilities[i] = binomialProbability<Type>(p, n, i);
    }
    Type x = uniform<Type>();
    Type sum = 0;
    for (uint8_t i = 0; i <= n; i++) {
        sum += probabilities[i];
        if (x <= sum) {
            return i;
        }
    }
    throw std::logic_error("Binomial distribution failed");
}

} // namespace Random
