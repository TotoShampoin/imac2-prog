#pragma once

#include "math/uniform.hpp"
#include "prog/Boid.hpp"
#include <vector>

class BoidContainer {
public:
    explicit BoidContainer(const size_t& amount);

    void update(const TotoGL::Seconds& delta);

    void resize(const size_t& amount);
    void resetBoids(const std::optional<size_t>& amount = std::nullopt);

    void addBoids(const size_t& = 1);
    void destroyBoids(const size_t& = 1);

    [[nodiscard]] const std::vector<Boid>& boids() const { return _boids; }

    [[nodiscard]] float cubeRadius() const { return _cube_radius; };
    [[nodiscard]] float minVelocity() const { return _min_velocity; };
    [[nodiscard]] float maxVelocity() const { return _max_velocity; };
    [[nodiscard]] const BoidForceParameters& boidForceParameters() const { return _boid_force_parameters; };
    [[nodiscard]] const BoidForce& cubeForce() const { return _cube_force; };

    float& cubeRadius() { return _cube_radius; };
    float& minVelocity() { return _min_velocity; };
    float& maxVelocity() { return _max_velocity; };
    BoidForceParameters& boidForceParameters() { return _boid_force_parameters; };
    BoidForce& cubeForce() { return _cube_force; };

    void updateCubeBoids(const glm::vec3& center);

private:
    std::vector<Boid> _boids;
    std::vector<Boid> _cube_boids { 6 };

    BoidForceParameters _boid_force_parameters {
        .avoid = { .force = .2, .zone_width = .5, .zone_offset = 0 },
        .match = { .force = .3, .zone_width = .5, .zone_offset = .5 },
        .center = { .force = .01, .zone_width = .5, .zone_offset = .5 }
    };
    BoidForce _cube_force { .force = 10, .zone_width = .3, .zone_offset = 0 };

    float _cube_radius { 5 };
    float _min_velocity { 1 };
    float _max_velocity { 2 };

    Random::Uniform<float> _rangom_number_generator { -1, 1 };
    void randomizeBoid(Boid& boid, const float& radius);
};
