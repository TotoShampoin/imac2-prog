#pragma once
#include <TotoGL/Primitives/Clock.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <optional>

/**
 * @brief A class for second order dynamics, following this video: https://www.youtube.com/watch?v=KPoeNZZ6H4s.
 *
 * @tparam T The type of the target. Boils down to a vector type.
 */
template <typename T>
class SecondOrderDynamics {
public:
    SecondOrderDynamics(const float& frequence, const float& damping, const float& response, const T& target = {})
        : _frequence(frequence)
        , _damping(damping)
        , _response(response)
        , _target_last(target)
        , _current(target)
        , _current_derivative({}) {
        updateFactors();
    }
    T update(const TotoGL::Seconds& delta, T& target, const std::optional<T>& __target_derivative = std::nullopt) {
        auto target_derivative = __target_derivative;
        if (!target_derivative.has_value()) {
            target_derivative = (target - _target_last) / delta;
            _target_last = target;
        }
        _current += _current_derivative * delta;
        _current_derivative += delta * (target + k3() * *target_derivative - _current - k1() * _current_derivative) / k2Stable(delta);
        return _current;
    }

    void reset(const T& target) {
        _target_last = target;
        _current = target;
        _current_derivative = {};
    }

    float k1() const { return _k1; }
    float k2() const { return _k2; }
    float k3() const { return _k3; }

    void changeFrequence(const float& frequence) {
        _frequence = frequence;
        updateFactors();
    }
    void changeDamping(const float& damping) {
        _damping = damping;
        updateFactors();
    }
    void changeResponse(const float& response) {
        _response = response;
        updateFactors();
    }
    void change(const float& frequence, const float& damping, const float& response) {
        _frequence = frequence;
        _damping = damping;
        _response = response;
        updateFactors();
    }

private:
    float _frequence;
    float _damping;
    float _response;
    float _k1;
    float _k2;
    float _k3;
    float k2Stable(const float& delta) const {
        return glm::max(_k2, (delta * delta) / 4 + delta * _k1 / 2);
    }
    void updateFactors() {
        static auto PI = glm::pi<float>();
        _k1 = _damping / (PI * _frequence);
        _k2 = 1 / ((2 * PI * _frequence) * (2 * PI * _frequence));
        _k3 = (_response * _damping) / (2 * PI * _frequence);
    }

    T _target_last;
    T _current;
    T _current_derivative;
};
