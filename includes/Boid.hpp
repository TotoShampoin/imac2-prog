#pragma once

#include <TotoGL.hpp>
#include <vector>

namespace ImacBoids {

// https://vanhunteradams.com/Pico/Animal_Movement/Boids-algorithm.htmlhttps://vanhunteradams.com/Pico/Animal_Movement/Boids-algorithm.html

class Boid {
public:
    Boid() = default;
    ~Boid() = default;

    void updateVelocity(const std::vector<Boid> &);
    void updateSeparation(const std::vector<Boid> &);
    void updateAlignment(const std::vector<Boid> &);
    void updateCohesion(const std::vector<Boid> &);
    void updatePosition(const TotoGL::Seconds &);

    const TotoGL::Vector3 &position() const { return _position; }
    const TotoGL::Vector3 &velocity() const { return _velocity; }

    void position(const TotoGL::Vector3 &);
    void velocity(const TotoGL::Vector3 &);

private:
    TotoGL::Vector3 _position;
    TotoGL::Vector3 _velocity;
    float _avoid_factor;
    float _matching_factor;
    float _centering_factor;
};

} // namespace ImacBoids