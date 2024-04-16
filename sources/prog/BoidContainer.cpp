#include "prog/BoidContainer.hpp"
#include <glm/common.hpp>
#include <glm/geometric.hpp>
#include <glm/glm.hpp>
#include <vector>

BoidContainer::BoidContainer(const size_t& amount, const std::function<void(Boid&)>& initializer)
    : _boids(amount) {
    resetBoids(amount, initializer);
}

void BoidContainer::update(const TotoGL::Seconds& delta) {
    for (auto& boid : _boids) {
        updateCubeBoids(boid.position());

        auto new_velocity = boid.velocity();
        new_velocity += boid.separation(_boids, boid.avoidForce());
        new_velocity += boid.alignment(_boids, boid.matchForce());
        new_velocity += boid.cohesion(_boids, boid.centerForce());
        new_velocity += boid.bias({ 0, 0, 0 });

        new_velocity += boid.separation(_cube_boids, _cube_force);

        boid.velocity() = glm::normalize(new_velocity) * glm::clamp(glm::length(new_velocity), _min_velocity, _max_velocity);
        boid.updatePosition(delta);

        if (glm::any(glm::isnan(boid.position()))) {
            boid.isAlive() = false;
        }
    }
    _boids.erase(
        std::remove_if(_boids.begin(), _boids.end(), [&](Boid& b) { return !b.isAlive(); }),
        _boids.end());
}

void BoidContainer::resize(const size_t& amount, const std::function<void(Boid&)>& initializer) {
    const auto old_size = _boids.size();
    _boids.resize(amount);
    for (auto i = old_size; i < amount; i++) {
        initializer(_boids[i]);
    }
}
void BoidContainer::resetBoids(const std::optional<size_t>& amount, const std::function<void(Boid&)>& initializer) {
    if (amount.has_value())
        resize(amount.value());

    for (auto& boid : _boids) {
        initializer(boid);
    }
}

void BoidContainer::addBoids(const size_t& amount, const std::function<void(Boid&)>& initializer) {
    for (int i = 0; i < amount; i++) {
        _boids.emplace_back();
        initializer(_boids.back());
    }
}
void BoidContainer::destroyBoids(const size_t& amount) {
    for (int i = 0; i < amount; i++) {
        if (_boids.empty())
            break;
        _boids.pop_back();
    }
}

void BoidContainer::updateCubeBoids(const glm::vec3& center) {
    static constexpr auto PUSH_DISTANCE = .1f;
    _cube_boids[0].position() = { center.x, center.y, _cube_radius };
    _cube_boids[1].position() = { center.x, center.y, -_cube_radius };
    _cube_boids[2].position() = { center.x, _cube_radius, center.z };
    _cube_boids[3].position() = { center.x, -_cube_radius, center.z };
    _cube_boids[4].position() = { _cube_radius, center.y, center.z };
    _cube_boids[5].position() = { -_cube_radius, center.y, center.z };
    for (auto& boid : _cube_boids) {
        boid.velocity() = glm::vec3(0);
        for (auto i = 0; i < 3; i++) {
            if (center[i] > _cube_radius)
                boid.position()[i] = center[i] + PUSH_DISTANCE;
            if (center[i] < -_cube_radius)
                boid.position()[i] = center[i] - PUSH_DISTANCE;
        }
    }
}
