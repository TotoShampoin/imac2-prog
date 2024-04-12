#include <gtest/gtest.h>
#include <math/random.hpp>

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

// This is to explicitly avoid factorials for this
TEST(Math, Binom_large) {
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
