#include "Programmation/Boid.hpp"
#include <glm/geometric.hpp>

// https://vanhunteradams.com/Pico/Animal_Movement/Boids-algorithm.html

Boid::Boid(
    const float &avoid, const float &match, const float &center,
    const TotoGL::Vector3 &pos,
    const TotoGL::Vector3 &vel)
    : _position(pos),
      _velocity(vel),
      _avoid_factor(avoid),
      _matching_factor(match),
      _centering_factor(center) {
}

void Boid::updateSeparation(const std::vector<Boid> &boids) {
    TotoGL::Vector3 close;
    for (const auto &boid : boids) {
        close += _position - boid._position;
    }
    _velocity += close * _avoid_factor;
}
void Boid::updateAlignment(const std::vector<Boid> &boids) {
    TotoGL::Vector3 sum_velocity;
    for (const auto &boid : boids) {
        sum_velocity += boid.velocity();
    }
    TotoGL::Vector3 avg_velocity = sum_velocity / float(boids.size());
    _velocity += (avg_velocity - _velocity) * _matching_factor;
}
void Boid::updateCohesion(const std::vector<Boid> &boids) {
    TotoGL::Vector3 sum_position;
    for (const auto &boid : boids) {
        sum_position += boid.position();
    }
    TotoGL::Vector3 avg_position = sum_position / float(boids.size());
    _velocity += (avg_position - _position) * _centering_factor;
}

void Boid::updateVelocity(const std::vector<Boid> &boids) {
    TotoGL::Vector3 close(0), avg_velocity(0), avg_position(0);
    for (const auto &boid : boids) {
        if (&boid == this)
            continue;
        close += _position - boid._position;
        avg_velocity += boid.velocity();
        avg_position += boid.position();
    }
    avg_velocity /= float(boids.size());
    avg_position /= float(boids.size());
    _velocity += close * _avoid_factor;
    _velocity += (avg_velocity - _velocity) * _matching_factor;
    _velocity += (avg_position - _position) * _centering_factor;
}

void Boid::updatePosition(const TotoGL::Seconds &delta_time) {
    float delta_time_s = delta_time.count();
    _position += _velocity * delta_time_s;
}

TotoGL::Vector3 Boid::positionLimited(const float &min, const float &max) const {
    return glm::mod(_position + 1.f, 2.f) - 1.f;
}