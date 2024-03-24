#pragma once

#include "TotoGL/Primitives/Clock.hpp"
#include <TotoGL/TotoGL.hpp>
#include <glm/fwd.hpp>

class Player {
public:
    glm::vec3& position() { return _position; }
    glm::vec3& velocity() { return _velocity; }

    glm::vec3 position() const { return _position; }
    glm::vec3 velocity() const { return _velocity; }

    void update(const TotoGL::Seconds& delta) {
        _position += _velocity * delta;
    }

    void move(glm::vec3 direction) {
        _velocity = direction;
    }

private:
    glm::vec3 _position;
    glm::vec3 _velocity;
};
