#pragma once
#include <cmath>
#include <cstdint>
#include <stdexcept>

namespace Math {

constexpr uint64_t factorial(uint8_t n) {
    if (n > 20) {
        throw std::overflow_error("Factorial overflow");
    }
    uint64_t result = 1;
    for (uint8_t i = 1; i <= n; i++) {
        result *= i;
    }
    return result;
}
constexpr uint64_t binom(uint8_t n, uint8_t k) {
    // return factorial(n) / (factorial(k) * factorial(n - k));
    if (k > n) {
        throw std::invalid_argument("k must be less than or equal to n");
    }
    if (k == 0) {
        return 1;
    }
    if (k > n / 2) {
        return binom(n, n - k);
    }
    return n * binom(n - 1, k - 1) / k;
}

template <typename Type>
constexpr Type binomialProbability(Type p, uint8_t n, uint8_t k) {
    return binom(n, k) * std::pow(p, k) * std::pow(1 - p, n - k);
}

} // namespace Math
