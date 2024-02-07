#include "prog/Boid.hpp"

// https://vanhunteradams.com/Pico/Animal_Movement/Boids-algorithm.html

Boid::Boid(
    const float& avoid, const float& match, const float& center,
    const glm::vec3& pos, const glm::vec3& vel)
    : _position(pos)
    , _velocity(vel)
    , _avoid_factor(avoid)
    , _matching_factor(match)
    , _centering_factor(center) { }

glm::vec3 Boid::separation(const std::vector<Boid>& boids) {
    if (boids.empty())
        return glm::vec3(0);
    glm::vec3 close_distance { 0 };
    for (const auto& other_boid : boids) {
        close_distance += position() - other_boid.position();
    }
    return close_distance * _avoid_factor;
}
glm::vec3 Boid::alignment(const std::vector<Boid>& boids) {
    if (boids.empty())
        return glm::vec3(0);
    glm::vec3 summed_velocity { 0 };
    for (const auto& other_boid : boids) {
        summed_velocity += other_boid.velocity();
    }
    glm::vec3 average_velocity = summed_velocity /= boids.size();
    return (average_velocity - velocity()) * _matching_factor;
}
glm::vec3 Boid::cohesion(const std::vector<Boid>& boids) {
    if (boids.empty())
        return glm::vec3(0);
    glm::vec3 summed_position { 0 };
    for (const auto& other_boid : boids) {
        summed_position += other_boid.position();
    }
    glm::vec3 average_position = summed_position /= boids.size();
    return (average_position - position()) * _centering_factor;
}

void Boid::updatePosition(const TotoGL::Seconds& delta_time) {
    _position += _velocity * delta_time;
}
