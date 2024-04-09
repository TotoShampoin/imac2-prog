#pragma once

#include "TotoGL/Primitives/Clock.hpp"
#include <TotoGL/TotoGL.hpp>
#include <glm/fwd.hpp>

class Player {
public:
    glm::vec3& position() { return _position; }
    glm::vec3& direction() { return _direction; }
    glm::vec3& velocity() { return _velocity; }

    glm::vec3 position() const { return _position; }
    glm::vec3 direction() const { return _direction; }
    glm::vec3 velocity() const { return _velocity; }

    void update(const TotoGL::Seconds& delta) {
        _position += _velocity * delta;
    }

    void move(const glm::vec3& direction) {
        if (glm::length(direction) > 0)
            _direction = glm::normalize(direction);
        _velocity = direction;
    }

private:
    glm::vec3 _position { 0 };
    glm::vec3 _direction { 0, 0, -1 };
    glm::vec3 _velocity { 0 };
};
