#pragma once

#include "math/base_rng.hpp"
#include "uniform.hpp"
#include <utility>
#include <vector>

namespace Random {

template <typename Type, typename RandomType = float>
class Enumerated : BaseRng<Type> {
public:
    explicit Enumerated(std::vector<std::pair<RandomType, Type>> values)
        : _values(std::move(values)) {
        for (auto& [prob, name] : _values) {
            _total += prob;
        }
    }

    Type operator()() {
        RandomType sum = 0;
        RandomType value = _rng();
        for (const auto& [prob, name] : _values) {
            sum += prob / _total;
            if (value < sum) {
                return name;
            }
        }
        return _values.back().second;
    }

    [[nodiscard]] size_t size() const { return _values.size(); }

private:
    Uniform<RandomType> _rng { 0, 1 };
    std::vector<std::pair<RandomType, Type>> _values;
    RandomType _total { 0 };
};

} // namespace Random
