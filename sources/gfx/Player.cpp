#include "gfx/Player.hpp"

void Player::update(const TotoGL::Seconds& delta) {
    _position += delta * (_velocity + _push);
}

void Player::move(const glm::vec3& direction, const TotoGL::Seconds& delta) {
    if (glm::length(direction) > 0)
        _direction = glm::normalize(direction);
    _velocity = glm::mix(_velocity, direction * _speed, delta);
}

void Player::push(const glm::vec3& force, const TotoGL::Seconds& delta) {
    _push = glm::mix(_push, force * _push_force * 2.f, delta);
}
