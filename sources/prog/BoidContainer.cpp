#include "prog/BoidContainer.hpp"
#include <glm/geometric.hpp>
#include <glm/glm.hpp>
#include <random>

BoidContainer::BoidContainer(const size_t& amount)
    : _boids(amount) {
    resetBoids();
}

void BoidContainer::update(const TotoGL::Seconds& delta) {
    for (auto& boid : _boids) {
        boid.velocity() += //
            boid.separation(_boids, _avoid_factor, _expell_radius, 0) + //
            boid.alignment(_boids, _matching_factor, _attract_radius, _expell_radius) + //
            boid.cohesion(_boids, _centering_factor, _attract_radius, _expell_radius);

        float* positions[] = { &boid.position().x, &boid.position().y, &boid.position().z };
        float* velocities[] = { &boid.velocity().x, &boid.velocity().y, &boid.velocity().z };
        for (int i = 0; i < 3; i++) {
            if (*positions[i] < -_cube_radius)
                *velocities[i] += delta * _returning_velocity;
            if (*positions[i] > _cube_radius)
                *velocities[i] -= delta * _returning_velocity;
        }

        if (glm::length(boid.velocity()) > _max_velocity)
            boid.velocity() = glm::normalize(boid.velocity()) * _max_velocity;

        boid.updatePosition(delta);

        if (glm::any(glm::isnan(boid.position()))) {
            auto distribution = std::uniform_real_distribution<float>(-1, 1);
            auto random = std::random_device();
            boid.position() = {
                distribution(random),
                distribution(random),
                distribution(random)
            };
            boid.velocity() = {
                distribution(random),
                distribution(random),
                distribution(random)
            };
        }
    }
}

void BoidContainer::resize(const size_t& amount) {
    auto distribution = std::uniform_real_distribution<float>(-1, 1);
    auto random = std::random_device();

    const auto old_size = _boids.size();
    _boids.resize(amount);
    for (auto i = old_size; i < amount; i++) {
        auto& boid = _boids[i];
        boid.position() = {
            distribution(random),
            distribution(random),
            distribution(random)
        };
        boid.velocity() = {
            distribution(random),
            distribution(random),
            distribution(random)
        };
    }
}
void BoidContainer::resetBoids(const std::optional<size_t>& amount) {
    auto distribution = std::uniform_real_distribution<float>(-1, 1);
    auto random = std::random_device();

    if (amount.has_value())
        resize(amount.value());

    for (auto& boid : _boids) {
        boid.position() = {
            distribution(random),
            distribution(random),
            distribution(random)
        };
        boid.velocity() = {
            distribution(random),
            distribution(random),
            distribution(random)
        };
    }
}
