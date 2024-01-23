#include "Programmation/Boid.hpp"

// https://vanhunteradams.com/Pico/Animal_Movement/Boids-algorithm.htmlhttps://vanhunteradams.com/Pico/Animal_Movement/Boids-algorithm.html

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
    TotoGL::Vector3 close, avg_velocity, avg_position;
    for (const auto &boid : boids) {
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

void Boid::position(const TotoGL::Vector3 &position) {
    _position = position;
}
void Boid::velocity(const TotoGL::Vector3 &velocity) {
    _velocity = velocity;
}
void Boid::avoid_factor(const float &avoid_factor) {
    _avoid_factor = avoid_factor;
}
void Boid::matching_factor(const float &matching_factor) {
    _matching_factor = matching_factor;
}
void Boid::centering_factor(const float &centering_factor) {
    _centering_factor = centering_factor;
}
