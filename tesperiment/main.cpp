#include "math/binomial.hpp"
#include <algorithm>
#include <iomanip>
#include <ios>
#include <iostream>
#include <math/random.hpp>
#include <vector>

constexpr int N = 1'000'000;
constexpr int RESOLUTION = 20;
constexpr int HISTOGRAM_HEIGHT = 160;

template <class RNG>
void testDistributionReal(RNG& rng, int n = N, int res = RESOLUTION) {
    std::vector<float> values(n);
    for (int i = 0; i < n; i++) {
        values[i] = rng();
    }
    float min = *std::min_element(values.begin(), values.end());
    float max = *std::max_element(values.begin(), values.end());
    float step = (max - min) / res;
    std::vector<int> histogram(res);
    for (int i = 0; i < n; i++) {
        int index = (values[i] - min) / step;
        histogram[index]++;
    }
    // leading spaces
    std::cout << std::setprecision(3) << std::fixed;
    for (int i = 0; i < res; i++) {
        std::cout << i * step + min << "\t" << histogram[i] << " \t";
        for (int j = 0; j < histogram[i] * HISTOGRAM_HEIGHT / n; j++) {
            std::cout << ">";
        }
        std::cout << std::endl;
    }
}

template <class RNG>
void testDistributionInt(RNG& rng, int n = N) {
    std::vector<int> values(n);
    for (int i = 0; i < n; i++) {
        values[i] = rng();
    }
    int min = *std::min_element(values.begin(), values.end());
    int max = *std::max_element(values.begin(), values.end());
    int res = max - min + 1;
    std::vector<int> histogram(res);
    for (int i = 0; i < n; i++) {
        histogram[values[i] - min]++;
    }
    // leading spaces
    std::cout << std::setprecision(3) << std::fixed;
    for (int i = 0; i < res; i++) {
        std::cout << i + min << "\t" << histogram[i] << " \t";
        for (int j = 0; j < histogram[i] * HISTOGRAM_HEIGHT / n; j++) {
            std::cout << ">";
        }
        std::cout << std::endl;
    }
}

int main(int argc, const char* argv[]) {
    Random::Uniform<float> uniform_dist(0, 1);
    Random::Exponential<float> exponential_dist(1);
    Random::Poisson<float> poisson_dist(3);
    Random::Binomial<float> binomial_dist(.1, RESOLUTION);
    Random::Normal<float> normal_dist;

    std::cout << "Uniform distribution [0,1]" << std::endl;
    testDistributionReal(uniform_dist);
    std::cout << "Exponential distribution (1)" << std::endl;
    testDistributionReal(exponential_dist);
    std::cout << "Poisson distribution (3)" << std::endl;
    testDistributionInt(poisson_dist, N);
    std::cout << "Binomial distribution (.1, 20)" << std::endl;
    testDistributionInt(binomial_dist);
    std::cout << "Normal distribution" << std::endl;
    testDistributionReal(normal_dist);

    return 0;
}