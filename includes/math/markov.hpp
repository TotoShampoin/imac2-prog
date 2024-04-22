#include "Eigen/Eigen"
#include "random/uniform.hpp"
#include <vector>

template <typename Type, size_t N>
class MarkovChain {
public:
    MarkovChain(const std::vector<Type>& values, const Eigen::Matrix<float, N, N>& probabilities)
        : _probabilities(probabilities)
        , _values(values) { }

    Type next() {
        auto next = _rng();
        for (size_t i = 0; i < N; i++) {
            next -= _probabilities(_current, i);
            if (next < 0) {
                _current = i;
                return _values[i];
            }
        }
        return _values[0];
    }

    [[nodiscard]] std::vector<float> nextProbabilities() const {
        std::vector<float> result(N);
        for (size_t i = 0; i < N; i++) {
            result[i] = _probabilities(_current, i);
        }
        return result;
    }

    [[nodiscard]] const std::vector<Type>& values() const { return _values; }
    [[nodiscard]] std::vector<Type>& values() { return _values; }
    [[nodiscard]] Type& current() { return _values[_current]; }
    [[nodiscard]] Type current() const { return _values[_current]; }
    [[nodiscard]] size_t currentIndex() const { return _current; }
    [[nodiscard]] const Eigen::Matrix<float, N, N>& probabilities() const { return _probabilities; }

private:
    Eigen::Matrix<float, N, N> _probabilities;
    Random::Uniform<float> _rng { 0, 1 };
    std::vector<Type> _values;
    std::size_t _current { 0 };
};
