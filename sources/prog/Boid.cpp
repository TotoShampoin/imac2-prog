#include "prog/Boid.hpp"
#include <glm/geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>

// https://vanhunteradams.com/Pico/Animal_Movement/Boids-algorithm.html

Boid::Boid(
    const float& avoid, const float& match, const float& center,
    const glm::vec3& pos, const glm::vec3& vel)
    : _position(pos)
    , _velocity(vel) { }

float Boid::closeness(const Boid& other, const float& radius, const float& offset) const {
    if (&other == this)
        return 0;
    if (radius == 0)
        return 1;
    float distance = glm::distance(position(), other.position());
    float distance_offset = glm::abs(glm::abs(distance) - offset);
    float distance_weighted = distance_offset * 2.f / radius;
    return glm::exp(-(distance_weighted * distance_weighted));
}

glm::vec3 Boid::separation(const std::vector<Boid>& boids, const float& avoid_factor,
    const float& attract_radius, const float& expell_radius) const {
    glm::vec3 close_distance { 0 };
    for (const auto& other_boid : boids) {
        auto cd = position() - other_boid.position();
        close_distance += cd * closeness(other_boid, attract_radius, expell_radius);
    }
    return close_distance * avoid_factor;
}
glm::vec3 Boid::alignment(const std::vector<Boid>& boids, const float& matching_factor,
    const float& attract_radius, const float& expell_radius) const {
    glm::vec3 summed_velocity { 0 };
    float weight = 0;
    for (const auto& other_boid : boids) {
        auto v = other_boid.velocity();
        auto c = closeness(other_boid, attract_radius, expell_radius);
        summed_velocity += v * c;
        weight += c;
    }
    glm::vec3 average_velocity = summed_velocity /= weight;
    return (average_velocity - velocity()) * matching_factor;
}
glm::vec3 Boid::cohesion(const std::vector<Boid>& boids, const float& centering_factor,
    const float& attract_radius, const float& expell_radius) const {
    glm::vec3 summed_position { 0 };
    float weight = 0;
    for (const auto& other_boid : boids) {
        auto p = other_boid.position();
        auto c = closeness(other_boid, attract_radius, expell_radius);
        summed_position += p * c;
        weight += c;
    }
    glm::vec3 average_position = summed_position /= weight;
    return (average_position - position()) * centering_factor;
}

void Boid::updatePosition(const TotoGL::Seconds& delta_time) {
    _position += _velocity * delta_time;
}
