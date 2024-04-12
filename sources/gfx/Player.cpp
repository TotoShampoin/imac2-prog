#include "gfx/Player.hpp"

void Player::update(const TotoGL::Seconds& delta) {
    _position += _velocity * delta;
}

void Player::move(const glm::vec3& direction, const TotoGL::Seconds& delta) {
    if (glm::length(direction) > 0)
        _direction = glm::normalize(direction);
    _velocity = glm::mix(_velocity, direction * _speed, delta);
}
