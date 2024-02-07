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

    glm::vec3 separation(const std::vector<Boid>&);
    glm::vec3 alignment(const std::vector<Boid>&);
    glm::vec3 cohesion(const std::vector<Boid>&);

    // void updateVelocity(const std::vector<Boid>&);
    void updatePosition(const TotoGL::Seconds&);

    glm::vec3 position() const { return _position; }
    glm::vec3 velocity() const { return _velocity; }
    float avoidFactor() const { return _avoid_factor; };
    float matchingFactor() const { return _matching_factor; };
    float centeringFactor() const { return _centering_factor; };

    glm::vec3& position() { return _position; }
    glm::vec3& velocity() { return _velocity; }
    float& avoidFactor() { return _avoid_factor; };
    float& matchingFactor() { return _matching_factor; };
    float& centeringFactor() { return _centering_factor; };

private:
    glm::vec3 _position;
    glm::vec3 _velocity;
    float _avoid_factor;
    float _matching_factor;
    float _centering_factor;
};
