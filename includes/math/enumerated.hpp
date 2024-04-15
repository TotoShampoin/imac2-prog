#pragma once

#include "uniform.hpp"
#include <map>

namespace Random {

template <typename Type, typename RandomType = float>
class Enumerated {
public:
    explicit Enumerated(std::map<RandomType, Type> values)
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
        return std::prev(_values.end())->second;
    }

private:
    Uniform<RandomType> _rng { 0, 1 };
    std::map<RandomType, Type> _values;
    RandomType _total { 0 };
};

} // namespace Random
