#pragma once

#include "TotoGL/Primitives/Clock.hpp"
#include "gfx/SecondOrderDynamics.hpp"
#include <TotoGL/TotoGL.hpp>
#include <glm/fwd.hpp>

class Player {
public:
    glm::vec3& position() { return _position; }
    glm::vec3& direction() { return _direction; }
    glm::vec3& frontDirection() { return _front_direction; }
    glm::vec3& velocity() { return _velocity; }
    glm::vec3& push() { return _push; }
    float& speed() { return _speed; }
    float& pushForce() { return _push_force; }

    [[nodiscard]] glm::vec3 direction() const { return *_target_dynamic; }

    [[nodiscard]] glm::vec3 position() const { return _position; }
    [[nodiscard]] glm::vec3 trueDirection() const { return _direction; }
    [[nodiscard]] glm::vec3 frontDirection() const { return _front_direction; }
    [[nodiscard]] glm::vec3 velocity() const { return _velocity; }
    [[nodiscard]] glm::vec3 push() const { return _push; }
    [[nodiscard]] float speed() const { return _speed; }
    [[nodiscard]] float pushForce() const { return _push_force; }

    void update(const TotoGL::Seconds& delta);
    void move(const glm::vec3& direction, const TotoGL::Seconds& delta);

    void push(const glm::vec3& force, const TotoGL::Seconds& delta);

private:
    glm::vec3 _position { 0 };
    glm::vec3 _direction { 0, 0, -1 };
    glm::vec3 _front_direction { 0, 0, -1 };
    glm::vec3 _velocity { 0 };
    glm::vec3 _push { 0 };
    float _speed { 10.f };
    float _push_force { 30.f };

    SecondOrderDynamics<glm::vec3> _target_dynamic { { 1, 1, 0 }, _direction };
};
