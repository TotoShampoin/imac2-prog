#pragma once

#include "math/variables.hpp"
#include "prog/Bait.hpp"
#include "prog/Boid.hpp"
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/glm.hpp>

struct SpawnerStats {
    float position_radius;
    BoidForceParameters boid_force_parameters;
    BoidSpeedCaps boid_speed_caps;
    float boid_speed_tendency;

    std::vector<glm::vec3> positions {};
    std::vector<glm::vec3> velocities {};
    std::vector<BoidForceParameters> forces {};
};

class BoidSpawner {
public:
    void spawnBoid(Boid& boid) {
        auto& _position_generator = Variables::instance()._boid_position_generator;
        auto& _angle_generator = Variables::instance()._boid_angle_generator;
        auto& _strength_generator = Variables::instance()._boid_strength_generator;
        auto& _color_generator = Variables::instance()._boid_color_generator;
        auto& _speed_generator = Variables::instance()._renderer_velocity_random;

        if (_boid_force_spread != _strength_generator.standardDeviation())
            _strength_generator.standardDeviation(_boid_force_spread);

        if (_speed_generator.parameter() != _boid_speed_tendency)
            _speed_generator.parameter(_boid_speed_tendency);

        auto random_position = glm::vec3(_position_generator(), _position_generator(), _position_generator());
        auto random_angles = _getRandom2D(_angle_generator);
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

        _stats.positions.push_back(boid.position());
        _stats.velocities.push_back(boid.velocity());
        _stats.forces.push_back(parameters);
    }

    void spawnBait(Bait& bait) {
        auto& _position_generator = Variables::instance()._boid_position_generator;
        auto random_position = glm::vec3(_position_generator(), _position_generator(), _position_generator());
        bait.position() = _position + random_position * _position_radius;
    }

    [[nodiscard]] glm::vec3& position() { return _position; }
    [[nodiscard]] glm::vec3& direction() { return _direction; }
    [[nodiscard]] float& positionRadius() { return _position_radius; }
    [[nodiscard]] BoidForceParameters& boidForceParameters() { return _boid_force_parameters; }
    [[nodiscard]] float& boidForceSpread() { return _boid_force_spread; }
    [[nodiscard]] BoidSpeedCaps& boidSpeedCaps() { return _boid_speed_caps; }
    [[nodiscard]] float& boidSpeedTendency() { return _boid_speed_tendency; }
    [[nodiscard]] SpawnerStats& stats() { return _stats; }

private:
    glm::vec2 _getRandom2D(Random::Normal<float>& rng) {
        auto [a, b] = rng.generatePair();
        return { a, b };
    }

    glm::vec3 _front() { return _direction; }
    glm::vec3 _right() { return glm::cross(_direction, { 0, 1, 0 }); }
    glm::vec3 _up() { return glm::cross(_right(), _front()); }

    glm::vec3 _position { 0 };
    glm::vec3 _direction { 0, 0, 1 };
    float _position_radius { .05 };
    BoidForceParameters _boid_force_parameters {
        .avoid = { .force = .2, .zone_width = .5, .zone_offset = 0 },
        .match = { .force = .3, .zone_width = .5, .zone_offset = .5 },
        .center = { .force = .1, .zone_width = .5, .zone_offset = .5 },
        .bias = { .force = .25, .zone_width = 1.5, .zone_offset = 0 },
    };
    float _boid_force_spread { .05 };
    BoidSpeedCaps _boid_speed_caps { .min = .5, .max = 1.5 };
    float _boid_speed_tendency { .5 };

    SpawnerStats _stats {
        .position_radius = _position_radius,
        .boid_force_parameters = _boid_force_parameters,
        .boid_speed_caps = _boid_speed_caps,
    };
};
