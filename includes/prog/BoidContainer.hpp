#pragma once

#include "prog/Boid.hpp"
#include <array>
#include <vector>

class BoidContainer {
public:
    BoidContainer(const size_t& amount);

    void update(const TotoGL::Seconds& delta);

    void resize(const size_t& amount);
    void resetBoids(const std::optional<size_t>& amount = std::nullopt);

    const std::vector<Boid>& boids() const { return _boids; }

    float avoidFactor() const { return _avoid_factor; };
    float matchingFactor() const { return _matching_factor; };
    float centeringFactor() const { return _centering_factor; };
    float cubeRadius() const { return _cube_radius; };
    float attractRadius() const { return _attract_radius; };
    float expellRadius() const { return _expell_radius; };
    float returningVelocity() const { return _returning_velocity; };
    float returningRadius() const { return _returning_radius; };
    float minVelocity() const { return _min_velocity; };
    float maxVelocity() const { return _max_velocity; };

    float& avoidFactor() { return _avoid_factor; };
    float& matchingFactor() { return _matching_factor; };
    float& centeringFactor() { return _centering_factor; };
    float& cubeRadius() { return _cube_radius; };
    float& attractRadius() { return _attract_radius; };
    float& expellRadius() { return _expell_radius; };
    float& returningVelocity() { return _returning_velocity; };
    float& returningRadius() { return _returning_radius; };
    float& minVelocity() { return _min_velocity; };
    float& maxVelocity() { return _max_velocity; };

    std::array<glm::vec3, 6> projectionsOnCube(const glm::vec3&) const;

private:
    std::vector<Boid> _boids;

    float _avoid_factor { .3 };
    float _matching_factor { .3 };
    float _centering_factor { .01 };
    float _returning_velocity { 10. };

    float _cube_radius { 5 };
    float _attract_radius { .5 };
    float _expell_radius { .5 };
    float _returning_radius { .3 };

    float _min_velocity { 1. };
    float _max_velocity { 2. };
};
