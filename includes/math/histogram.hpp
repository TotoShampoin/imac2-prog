#pragma once

#include <functional>
#include <limits>
#include <vector>

template <typename T>
struct ProbabilityHistogram {
    std::vector<float> histogram;
    float max_count { 0 };
    float min_value { 0 };
    float max_value { 0 };
    size_t HISTOGRAM_SIZE;
    float expected_value { 0 };
    float variance { 0 };

    explicit ProbabilityHistogram(
        const size_t& size,
        const std::vector<T>& values,
        const std::function<float(const T&)>& value_getter)
        : histogram(size, 0)
        , HISTOGRAM_SIZE(size) {
        recalculate(values, value_getter);
    }

    void recalculate(const std::vector<T>& values, const std::function<float(const T&)>& value_getter) {
        max_count = 0;
        min_value = 0;
        max_value = 0;
        std::fill(histogram.begin(), histogram.end(), 0);
        if (!values.empty()) {
            max_count = 0;
            min_value = std::numeric_limits<float>::max();
            max_value = std::numeric_limits<float>::min();
            expected_value = 0;
            float expected_value_squared = 0;
            variance = 0;
            for (const auto& value : values) {
                auto v = value_getter(value);
                min_value = std::min(min_value, v);
                max_value = std::max(max_value, v);
                expected_value += v;
                expected_value_squared += v * v;
            }
            expected_value /= values.size();
            variance = expected_value_squared / values.size() - expected_value * expected_value;
            for (const auto& value : values) {
                auto v = value_getter(value);
                auto index = static_cast<size_t>((v - min_value) / (max_value - min_value) * (HISTOGRAM_SIZE - 1));
                histogram[index] += 1;
                max_count = std::max(max_count, histogram[index]);
            }
        }
    }
};
