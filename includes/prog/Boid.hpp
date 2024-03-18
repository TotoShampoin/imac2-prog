#pragma once

#include "TotoGL/Primitives/Clock.hpp"
#include <glm/glm.hpp>
#include <vector>

// https://vanhunteradams.com/Pico/Animal_Movement/Boids-algorithm.html

class Boid {
public:
    Boid(
        const float& avoid = .05,
        const float& match = .05,
        const float& center = .0005,
        const glm::vec3& position = glm::vec3(0, 0, 0),
        const glm::vec3& velocity = glm::vec3(0, 0, 1));
    ~Boid() = default;

    float closeness(const Boid&, const float& radius, const float& offset) const;

    glm::vec3 separation(const std::vector<Boid>&, const float& avoid_factor,
        const float& attract_radius, const float& expell_radius) const;
    glm::vec3 alignment(const std::vector<Boid>&, const float& matching_factor,
        const float& attract_radius, const float& expell_radius) const;
    glm::vec3 cohesion(const std::vector<Boid>&, const float& centering_factor,
        const float& attract_radius, const float& expell_radius) const;

    // void updateVelocity(const std::vector<Boid>&);
    void updatePosition(const TotoGL::Seconds&);

    glm::vec3 position() const { return _position; }
    glm::vec3 velocity() const { return _velocity; }

    glm::vec3& position() { return _position; }
    glm::vec3& velocity() { return _velocity; }

private:
    glm::vec3 _position;
    glm::vec3 _velocity;
};
