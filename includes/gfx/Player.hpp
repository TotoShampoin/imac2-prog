#pragma once

#include <TotoGL/TotoGL.hpp>

class Player {
public:
    Player(TotoGL::RenderObjectInstanceId object)
        : _object(object) { }

    glm::vec3& position() { return _object->position(); }
    glm::vec3& velocity() { return _velocity; }

    void update(float dt) {
        position() += _velocity * dt;
    }

    void move(glm::vec3 direction) {
        _velocity = direction;
    }

private:
    TotoGL::RenderObjectInstanceId _object;
    glm::vec3 _velocity;
};
