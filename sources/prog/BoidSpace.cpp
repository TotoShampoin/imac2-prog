#include "prog/BoidSpace.hpp"
#include <algorithm>
#include <glm/geometric.hpp>
#include <glm/glm.hpp>
#include <random>

BoidSpace::BoidSpace(const size_t& amount)
    : _boids(amount) {
    resetBoids();
}

void BoidSpace::update(const TotoGL::Seconds& delta) {
    static constexpr float close_radius = .125;
    static constexpr float steer_angle_per_second = glm::radians(120.);
    static constexpr float returning_velocity = 1.;
    static constexpr float max_velocity = 2.;

    for (auto& boid : _boids) {
        auto all_except_this = std::vector<Boid>();
        std::copy_if(
            _boids.begin(), _boids.end(), std::back_inserter(all_except_this),
            [&](Boid& other_boid) {
                return &boid != &other_boid;
            });

        boid.velocity() += //
            boid.separation(all_except_this, _avoid_factor, close_radius) + //
            boid.alignment(all_except_this, _matching_factor, close_radius) + //
            boid.cohesion(all_except_this, _centering_factor, close_radius);

        // Attract away from bounds
        float* positions[] = { &boid.position().x, &boid.position().y, &boid.position().z };
        float* velocities[] = { &boid.velocity().x, &boid.velocity().y, &boid.velocity().z };
        for (int i = 0; i < 3; i++) {
            if (*positions[i] < -_cube_radius)
                *velocities[i] += delta * returning_velocity;
            if (*positions[i] > _cube_radius)
                *velocities[i] -= delta * returning_velocity;
        }

        if (glm::length(boid.velocity()) > max_velocity)
            boid.velocity() = glm::normalize(boid.velocity()) * max_velocity;

        boid.updatePosition(delta);

        // if nan, random position
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

void BoidSpace::resize(const size_t& amount) {
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
void BoidSpace::resetBoids(const std::optional<size_t>& amount) {
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
