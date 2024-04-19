#include "prog/Boid.hpp"
#include <glm/common.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>

// https://vanhunteradams.com/Pico/Animal_Movement/Boids-algorithm.html

Boid::Boid(
    // const glm::vec3& pos, const glm::vec3& vel,
    const BoidCoordinates& coords,
    const BoidForceParameters& forces,
    const BoidSpeedCaps& speed,
    const TotoGL::ColorRGB& color)
    : _position(coords.position)
    , _velocity(coords.velocity)
    , _avoid_force(forces.avoid)
    , _match_force(forces.match)
    , _center_force(forces.center)
    , _bias_force(forces.bias)
    , _min_speed(speed.min)
    , _max_speed(speed.max)
    , _color(color)
    , _is_alive(true)
    , _lifetime(0) { }

float Boid::closeness(const Boid& other, const BoidForce& force) const {
    const auto& radius = force.zone_width;
    const auto& offset = force.zone_offset;
    if (&other == this)
        return 0;
    if (radius == 0)
        return 1;
    float distance = glm::distance(position(), other.position());
    float distance_offset = glm::abs(glm::abs(distance) - offset);
    float distance_weighted = distance_offset * 2.f / radius;
    return glm::exp(-(distance_weighted * distance_weighted));
}

// meant to be a sum
glm::vec3 Boid::separation(const std::vector<Boid>& boids, const BoidForce& avoid_force) const {
    const auto& avoid_factor = avoid_force.force;
    glm::vec3 close_distance { 0 };
    for (const auto& other_boid : boids) {
        auto d = position() - other_boid.position();
        auto c = closeness(other_boid, avoid_force);
        close_distance += d * c;
    }
    return close_distance * avoid_factor;
}

// meant to be an average
glm::vec3 Boid::alignment(const std::vector<Boid>& boids, const BoidForce& match_force) const {
    const auto& match_factor = match_force.force;
    glm::vec3 summed_velocity { 0 };
    float weight = 0;
    for (const auto& other_boid : boids) {
        auto v = other_boid.velocity();
        auto c = closeness(other_boid, match_force);
        summed_velocity += v * c;
        weight += c;
    }
    if (weight == 0)
        return glm::vec3(0);
    glm::vec3 average_velocity = summed_velocity /= weight;
    return (average_velocity - velocity()) * match_factor;
}

// meant to be an average
glm::vec3 Boid::cohesion(const std::vector<Boid>& boids, const BoidForce& center_force) const {
    const auto& center_factor = center_force.force;
    glm::vec3 summed_position { 0 };
    float weight = 0;
    for (const auto& other_boid : boids) {
        auto p = other_boid.position();
        auto c = closeness(other_boid, center_force);
        summed_position += p * c;
        weight += c;
    }
    if (weight == 0)
        return glm::vec3(0);
    glm::vec3 average_position = summed_position /= weight;
    return (average_position - position()) * center_factor;
}

// meant to be a sum
glm::vec3 Boid::bias(const std::vector<Bait>& baits, const BoidForce& bias_force) const {
    const auto& bias_factor = bias_force.force;
    glm::vec3 summed_bias { 0 };
    for (const auto& bait : baits) {
        auto d = glm::normalize(bait.position() - position());
        auto c = closeness(Boid({ bait.position() }), bias_force);
        summed_bias += d * c;
    }
    return summed_bias * bias_factor;
}

glm::vec3 Boid::bias(const glm::vec3& point) const {
    auto c = closeness(Boid({ point }), _bias_force);
    auto direction = glm::normalize(point - _position);
    return direction * c * _bias_force.force;
}

void Boid::updatePosition(const TotoGL::Seconds& delta_time) {
    if (glm::length(_velocity) > 0)
        _velocity = glm::normalize(_velocity) * glm::clamp(glm::length(_velocity), _min_speed, _max_speed);
    _position += _velocity * delta_time;
    _lifetime += delta_time;
}

void Boid::setParameters(const BoidForceParameters& forces) {
    _avoid_force = forces.avoid;
    _match_force = forces.match;
    _center_force = forces.center;
    _bias_force = forces.bias;
}
void Boid::setSpeedCaps(const BoidSpeedCaps& speed) {
    _min_speed = speed.min;
    _max_speed = speed.max;
}
