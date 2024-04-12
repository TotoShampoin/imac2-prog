#include <gtest/gtest.h>
#include <math/random.hpp>

TEST(Math, Factorial) {
    EXPECT_EQ(Math::factorial(4), 24UL);
    EXPECT_EQ(Math::factorial(8), 40320UL);
    EXPECT_EQ(Math::factorial(12), 479001600UL);
    EXPECT_EQ(Math::factorial(16), 20922789888000UL);
    EXPECT_EQ(Math::factorial(20), 2432902008176640000UL);
    EXPECT_THROW(Math::factorial(21), std::overflow_error);
}

TEST(Math, Binom) {
    EXPECT_EQ(Math::binom(4, 2), 6UL);
    EXPECT_EQ(Math::binom(8, 3), 56UL);
    EXPECT_EQ(Math::binom(12, 5), 792UL);
    EXPECT_EQ(Math::binom(16, 8), 12870UL);
    EXPECT_EQ(Math::binom(20, 10), 184756UL);
}

// Expect this to fail when using factorials
// If it doesn't, that means you're not using factorials
// which means good job :)
TEST(Math, Binom_large) {
    EXPECT_EQ(Math::binom(40, 20), 137846528820UL);
    EXPECT_EQ(Math::binom(50, 25), 126410606437752UL);
    EXPECT_EQ(Math::binom(60, 30), 118264581564861424UL);
}
