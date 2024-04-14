#pragma once

#include "TotoGL/Primitives/Clock.hpp"
#include <TotoGL/TotoGL.hpp>
#include <glm/fwd.hpp>

class Player {
public:
    glm::vec3& position() { return _position; }
    glm::vec3& direction() { return _direction; }
    glm::vec3& velocity() { return _velocity; }
    float& speed() { return _speed; }

    [[nodiscard]] glm::vec3 position() const { return _position; }
    [[nodiscard]] glm::vec3 direction() const { return _direction; }
    [[nodiscard]] glm::vec3 velocity() const { return _velocity; }
    [[nodiscard]] float speed() const { return _speed; }

    void update(const TotoGL::Seconds& delta);
    void move(const glm::vec3& direction, const TotoGL::Seconds& delta);

private:
    glm::vec3 _position { 0 };
    glm::vec3 _direction { 0, 0, -1 };
    glm::vec3 _velocity { 0 };
    float _speed { 10.f };
};
