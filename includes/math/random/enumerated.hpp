#pragma once

#include "base_rng.hpp"
#include "uniform.hpp"
#include <algorithm>
#include <utility>
#include <vector>
// #include <map>

namespace Random {

template <typename Type, typename RandomType = float>
class Enumerated : BaseRng<Type> {
public:
    explicit Enumerated(std::vector<std::pair<Type, RandomType>> values)
        : _values(std::move(values)) {
        recalculateTotal();
    }

    Type operator()() {
        RandomType sum = 0;
        RandomType value = _rng();
        for (const auto& [name, prob] : _values) {
            sum += prob / _total;
            if (value < sum) {
                return name;
            }
        }
        return std::prev(_values.end())->first;
    }

    [[nodiscard]] std::vector<std::pair<Type, RandomType>>& values() { return _values; }
    [[nodiscard]] size_t size() const { return _values.size(); }
    [[nodiscard]] RandomType total() const { return _total; }

    void set(const Type& name, RandomType prob) {
        auto it = get(name);
        if (it != _values.end()) {
            it->second = prob;
        } else {
            _values.emplace_back(name, prob);
        }
        recalculateTotal();
    }

    void recalculateTotal() {
        _total = 0;
        for (auto& [name, prob] : _values) {
            _total += prob;
        }
    }

private:
    Uniform<RandomType> _rng { 0, 1 };
    std::vector<std::pair<Type, RandomType>> _values;
    RandomType _total { 0 };

    auto get(const Type& name) {
        return std::find_if(_values.begin(), _values.end(), [&](const auto& pair) {
            return pair.first == name;
        });
    }
};

} // namespace Random
