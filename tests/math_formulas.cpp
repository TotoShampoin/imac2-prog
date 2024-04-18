#include <algorithm>
#include <gtest/gtest.h>
#include <math/random/random.hpp>

TEST(Math, Factorial) {
    // Generated with math.factorial in Python
    auto units = std::map<uint8_t, uint64_t> {
        { 4, 24 },
        { 8, 40320 },
        { 12, 479001600 },
        { 16, 20922789888000 },
        { 20, 2432902008176640000 }
    };
    for (auto& t : units) {
        EXPECT_EQ(Math::factorial(t.first), t.second);
    }
    EXPECT_THROW(Math::factorial(21), std::overflow_error);
}

TEST(Math, Binom) {
    // Generated with math.comb in Python
    auto units = std::map<uint8_t, uint64_t> {
        { 4, 6 },
        { 8, 70 },
        { 12, 924 },
        { 16, 12870 },
        { 20, 184756 }
    };
    for (auto& t : units) {
        EXPECT_EQ(Math::binom(t.first, t.first / 2), t.second);
    }
}

// If this fails, that means you're probably using factorials. Please don't.
TEST(Math, BinomLarge) {
    // Generated with math.comb in Python
    auto units = std::map<uint8_t, uint64_t> {
        { 40, 137846528820 },
        { 50, 126410606437752 },
        { 60, 118264581564861424 },
        { 62, 465428353255261088 }
    };
    for (auto& t : units) {
        EXPECT_EQ(Math::binom(t.first, t.first / 2), t.second);
    }
}

TEST(Math, BinomialProbability) {
    // Generated with math.comb and ** in Python
    auto units = std::map<std::tuple<float, uint8_t, uint8_t>, float> {
        { { .5f, 4, 2 }, .375f },
        { { .5f, 8, 4 }, .2734375f },
        { { .5f, 12, 6 }, .2255859375f },
        { { .5f, 16, 8 }, .196380615234375f },
        { { .5f, 20, 10 }, .17619705200195312f }
    };
    const float ERR = 1e-9;
    for (auto& t : units) {
        const auto& [p, n, k] = t.first;
        EXPECT_NEAR(Math::binomialProbability(p, n, k), t.second, ERR);
    }
}

constexpr int N = 1'000'000;
constexpr float ERR = .01f;

template <class RNG>
void test_rng(RNG& rng, float expected, float variance) {
    double sum = 0;
    double sum2 = 0;
    for (int i = 0; i < N; i++) {
        auto x = rng();
        sum += x;
        sum2 += x * x;
    }
    sum /= N;
    sum2 /= N;
    double var = sum2 - sum * sum;
    EXPECT_NEAR(sum, expected, ERR);
    EXPECT_NEAR(var, variance, ERR);
}

TEST(Random, Uniform) {
    const float A = 0;
    const float B = 1;
    auto rng = Random::Uniform<float>(A, B);
    auto expected = (A + B) / 2;
    auto variance = (B - A) * (B - A) / 12;
    test_rng(rng, expected, variance);
}

TEST(Random, Exponential) {
    const float P = 1;
    auto rng = Random::Exponential<float>(P);
    auto expected = 1 / P;
    auto variance = 1 / (P * P);
    test_rng(rng, expected, variance);
}

TEST(Random, Poisson) {
    const float L = 1;
    auto rng = Random::Poisson<float>(L);
    auto expected = L;
    auto variance = L;
    test_rng(rng, expected, variance);
}

TEST(Random, Normal) {
    auto rng = Random::Normal<float>();
    auto expected = 0.f;
    auto variance = 1.f;
    test_rng(rng, expected, variance);
}

// TODO(Random) Test Enumerated
TEST(Random, Enumerate) {
    enum TestType {
        OPTION_A,
        OPTION_B,
        OPTION_C,
        OPTION_D,
    };
    auto distribution = std::vector<std::pair<float, TestType>>({
        { .1, OPTION_A },
        { .3, OPTION_B },
        { .2, OPTION_C },
        { .4, OPTION_D },
    });
    auto rng = Random::Enumerated<TestType>(distribution);
    auto results = std::map<TestType, int>();
    for (int i = 0; i < N; i++) {
        auto val = rng();
        results[val]++;
    }
    for (auto& [value, count] : results) {
        // auto expected = distribution.find(value)->first;
        auto expected = std::find_if(distribution.begin(), distribution.end(), [value](auto& p) {
            return p.second == value;
        })->first;
        auto actual = static_cast<float>(count) / N;
        EXPECT_NEAR(expected, actual, ERR);
    }
}
