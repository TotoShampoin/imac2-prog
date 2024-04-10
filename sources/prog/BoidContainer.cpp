#include "prog/BoidContainer.hpp"
#include <glm/common.hpp>
#include <glm/geometric.hpp>
#include <glm/glm.hpp>
#include <random>

void randomizeBoid(Boid& boid, const float& radius) {
    static std::uniform_real_distribution<float> _distribution { -1, 1 };
    static std::random_device _random {};

    boid.position() = {
        _distribution(_random) * radius,
        _distribution(_random) * radius,
        _distribution(_random) * radius
    };
    boid.velocity() = {
        _distribution(_random),
        _distribution(_random),
        _distribution(_random)
    };
}

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

        { // Obstacle, atempt #1 : fake boids at the cube's walls
            auto fake_boids = std::vector<Boid>(6);
            for (auto projection : projectionsOnCube(boid.position())) {
                fake_boids.push_back(Boid(0, 0, 0, projection, glm::vec3(0)));
            }
            boid.velocity() += boid.separation(fake_boids, _returning_velocity, _returning_radius, 0);
        }

        if (glm::length(boid.velocity()) > _max_velocity)
            boid.velocity() = glm::normalize(boid.velocity()) * _max_velocity;
        if (glm::length(boid.velocity()) < _min_velocity)
            boid.velocity() = glm::normalize(boid.velocity()) * _min_velocity;

        boid.updatePosition(delta);

        if (glm::any(glm::isnan(boid.position()))) {
            randomizeBoid(boid, _cube_radius);
        }
    }
}

void BoidContainer::resize(const size_t& amount) {
    const auto old_size = _boids.size();
    _boids.resize(amount);
    for (auto i = old_size; i < amount; i++) {
        randomizeBoid(_boids[i], _cube_radius);
    }
}
void BoidContainer::resetBoids(const std::optional<size_t>& amount) {
    if (amount.has_value())
        resize(amount.value());

    for (auto& boid : _boids) {
        randomizeBoid(boid, _cube_radius);
    }
}

std::array<glm::vec3, 6> BoidContainer::projectionsOnCube(const glm::vec3& point) const {
    auto results = std::array<glm::vec3, 6>({ glm::vec3(point.x, point.y, _cube_radius),
        glm::vec3(point.x, point.y, -_cube_radius),
        glm::vec3(point.x, _cube_radius, point.z),
        glm::vec3(point.x, -_cube_radius, point.z),
        glm::vec3(_cube_radius, point.y, point.z),
        glm::vec3(-_cube_radius, point.y, point.z) });
    // if point is outside the cube, place the 3 closest projections in front of the point, such that the walls are always around the point
    for (auto& result : results) {
        if (point.x > _cube_radius)
            result.x = point.x + .1f;
        if (point.x < -_cube_radius)
            result.x = point.x - .1f;
        if (point.y > _cube_radius)
            result.y = point.y + .1f;
        if (point.y < -_cube_radius)
            result.y = point.y - .1f;
        if (point.z > _cube_radius)
            result.z = point.z + .1f;
        if (point.z < -_cube_radius)
            result.z = point.z - .1f;
    }
    return results;
}
