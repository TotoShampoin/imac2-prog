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

    void updateSeparation(const std::vector<Boid>&);
    void updateAlignment(const std::vector<Boid>&);
    void updateCohesion(const std::vector<Boid>&);

    // Performs separation, alignment and cohesion all at once, in one loop
    void updateVelocity(const std::vector<Boid>&);
    void updatePosition(const TotoGL::Seconds&);

    glm::vec3 position() const { return _position; }
    glm::vec3 positionLimited(const float& min = -1, const float& max = 1) const;
    glm::vec3 velocity() const { return _velocity; }
    float avoid_factor() const { return _avoid_factor; };
    float matching_factor() const { return _matching_factor; };
    float centering_factor() const { return _centering_factor; };

    glm::vec3& position() { return _position; }
    glm::vec3& velocity() { return _velocity; }
    float& avoid_factor() { return _avoid_factor; };
    float& matching_factor() { return _matching_factor; };
    float& centering_factor() { return _centering_factor; };

private:
    glm::vec3 _position;
    glm::vec3 _velocity;
    float _avoid_factor;
    float _matching_factor;
    float _centering_factor;
};
