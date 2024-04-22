#include "prog/BoidSpawner.hpp"
#include "math/variables.hpp"

glm::vec2 getRandom2D(Random::Normal<float>& rng) {
    auto [a, b] = rng.generatePair();
    return { a, b };
}

void BoidSpawner::checkParameters() {
    auto& _strength_generator = Variables::instance()._boid_strength_generator;
    auto& _speed_generator = Variables::instance()._boid_velocity_random;
    if (_boid_force_spread != _strength_generator.standardDeviation()) {
        _strength_generator.standardDeviation(_boid_force_spread);
    }
    if (_speed_generator.parameter() != _boid_speed_tendency) {
        _speed_generator.parameter(_boid_speed_tendency);
    }

    if (_stats.position_radius != _position_radius) {
        _stats.position_radius = _position_radius;
        _stats.positions.clear();
    }
    if (
        _stats.boid_force_parameters.avoid.force != _boid_force_parameters.avoid.force || //
        _stats.boid_force_parameters.match.force != _boid_force_parameters.match.force || //
        _stats.boid_force_parameters.center.force != _boid_force_parameters.center.force || //
        _stats.boid_force_parameters.bias.force != _boid_force_parameters.bias.force || //
        _stats.boid_speed_tendency != _boid_speed_tendency //
    ) {
        _stats.boid_force_parameters = _boid_force_parameters;
        _stats.boid_speed_tendency = _boid_speed_tendency;
        _stats.forces.clear();
    }
    if (
        _stats.boid_speed_caps.min != _boid_speed_caps.min || //
        _stats.boid_speed_caps.max != _boid_speed_caps.max || //
        _stats.boid_speed_tendency != _boid_speed_tendency //
    ) {
        _stats.boid_speed_caps = _boid_speed_caps;
        _stats.boid_speed_tendency = _boid_speed_tendency;
        _stats.velocities.clear();
    }
}

void BoidSpawner::spawnBoid(Boid& boid) {
    auto& _position_generator = Variables::instance()._boid_position_generator;
    auto& _angle_generator = Variables::instance()._boid_angle_generator;
    auto& _strength_generator = Variables::instance()._boid_strength_generator;
    auto& _color_generator = Variables::instance()._boid_color_generator;
    auto& _speed_generator = Variables::instance()._boid_velocity_random;

    checkParameters();

    auto random_position = glm::vec3(_position_generator(), _position_generator(), _position_generator());
    auto random_angles = getRandom2D(_angle_generator);
    auto random_speed = glm::mix(_boid_speed_caps.min, _boid_speed_caps.max, _speed_generator.normalized());
    auto color = _color_generator();

    auto rotated = glm::rotate(glm::mat4(1), random_angles.x, _up()) * glm::rotate(glm::mat4(1), random_angles.y, _right());
    BoidForceParameters parameters = _boid_force_parameters;
    parameters.avoid.force += _strength_generator();
    parameters.center.force += _strength_generator();
    parameters.match.force += _strength_generator();
    parameters.bias.force += _strength_generator();

    boid.position() = _position + random_position * _position_radius;
    boid.velocity() = (rotated * glm::vec4(_direction, 1)) * random_speed;
    boid.setParameters(parameters);
    boid.setSpeedCaps(_boid_speed_caps);
    boid.color() = color;

    _stats.positions.push_back(boid.position());
    _stats.velocities.push_back(boid.velocity());
    _stats.forces.push_back(parameters);
}

void BoidSpawner::spawnBait(Bait& bait) {
    auto& _position_generator = Variables::instance()._boid_position_generator;
    auto random_position = glm::vec3(_position_generator(), _position_generator(), _position_generator());
    bait.position() = _position + random_position * _position_radius;
}
