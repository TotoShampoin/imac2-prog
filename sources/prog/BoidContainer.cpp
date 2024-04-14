#include "prog/BoidContainer.hpp"
#include "math/uniform.hpp"
#include <glm/common.hpp>
#include <glm/geometric.hpp>
#include <glm/glm.hpp>

void BoidContainer::randomizeBoid(Boid& boid, const float& radius) {
    boid.position() = {
        _rangom_number_generator() * radius,
        _rangom_number_generator() * radius,
        _rangom_number_generator() * radius
    };
    boid.velocity() = {
        _rangom_number_generator(),
        _rangom_number_generator(),
        _rangom_number_generator()
    };
    boid.setParameters(_boid_force_parameters);
}

BoidContainer::BoidContainer(const size_t& amount)
    : _boids(amount) {
    resetBoids();
}

void BoidContainer::update(const TotoGL::Seconds& delta) {
    for (auto& boid : _boids) {
        auto new_velocity = boid.velocity();
        new_velocity += boid.separation(_boids, boid.avoidForce());
        new_velocity += boid.alignment(_boids, boid.matchForce());
        new_velocity += boid.cohesion(_boids, boid.centerForce());

        {
            static auto fake_boids = std::vector<Boid>(6);
            auto projections = projectionsOnCube(boid.position());
            for (const auto& projection : projections) {
                std::ptrdiff_t index = &projection - projections.data();
                fake_boids[index].position() = projection;
                fake_boids[index].velocity() = glm::vec3(0);
            }
            new_velocity += boid.separation(fake_boids, _cube_force);
        }

        if (glm::length(new_velocity) > _max_velocity)
            new_velocity = glm::normalize(new_velocity) * _max_velocity;
        if (glm::length(new_velocity) < _min_velocity)
            new_velocity = glm::normalize(new_velocity) * _min_velocity;

        boid.velocity() = new_velocity;
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
    static constexpr auto PUSH_DISTANCE = .1f;
    auto results = std::array<glm::vec3, 6>({ glm::vec3(point.x, point.y, _cube_radius),
        glm::vec3(point.x, point.y, -_cube_radius),
        glm::vec3(point.x, _cube_radius, point.z),
        glm::vec3(point.x, -_cube_radius, point.z),
        glm::vec3(_cube_radius, point.y, point.z),
        glm::vec3(-_cube_radius, point.y, point.z) });
    for (auto& result : results) {
        for (auto i = 0; i < 3; i++) {
            if (result[i] > _cube_radius)
                result[i] = point[i] + PUSH_DISTANCE;
            if (result[i] < -_cube_radius)
                result[i] = point[i] - PUSH_DISTANCE;
        }
    }
    return results;
}
