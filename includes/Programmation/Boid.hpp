#pragma once

#include <TotoGL.hpp>
#include <vector>

// https://vanhunteradams.com/Pico/Animal_Movement/Boids-algorithm.html

class Boid {
public:
    Boid(
        const float &avoid = .05,
        const float &match = .05,
        const float &center = .0005,
        const TotoGL::Vector3 &position = TotoGL::Vector3(0, 0, 0),
        const TotoGL::Vector3 &velocity = TotoGL::Vector3(0, 0, 1));
    ~Boid() = default;

    void updateSeparation(const std::vector<Boid> &);
    void updateAlignment(const std::vector<Boid> &);
    void updateCohesion(const std::vector<Boid> &);

    // Performs separation, alignment and cohesion all at once, in one loop
    void updateVelocity(const std::vector<Boid> &);
    void updatePosition(const TotoGL::Seconds &);

    const TotoGL::Vector3 &position() const { return _position; }
    TotoGL::Vector3 positionLimited(const float &min = -1, const float &max = 1) const;
    const TotoGL::Vector3 &velocity() const { return _velocity; }
    const float &avoid_factor() const { return _avoid_factor; };
    const float &matching_factor() const { return _matching_factor; };
    const float &centering_factor() const { return _centering_factor; };

    TotoGL::Vector3 &position() { return _position; }
    TotoGL::Vector3 &velocity() { return _velocity; }
    float &avoid_factor() { return _avoid_factor; };
    float &matching_factor() { return _matching_factor; };
    float &centering_factor() { return _centering_factor; };

private:
    TotoGL::Vector3 _position;
    TotoGL::Vector3 _velocity;
    float _avoid_factor;
    float _matching_factor;
    float _centering_factor;
};
