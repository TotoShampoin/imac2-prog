#pragma once

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
    void spawnBoid(Boid& boid);

    void spawnBait(Bait& bait);

    [[nodiscard]] glm::vec3& position() { return _position; }
    [[nodiscard]] glm::vec3& direction() { return _direction; }
    [[nodiscard]] float& positionRadius() { return _position_radius; }
    [[nodiscard]] BoidForceParameters& boidForceParameters() { return _boid_force_parameters; }
    [[nodiscard]] float& boidForceSpread() { return _boid_force_spread; }
    [[nodiscard]] BoidSpeedCaps& boidSpeedCaps() { return _boid_speed_caps; }
    [[nodiscard]] float& boidSpeedTendency() { return _boid_speed_tendency; }
    [[nodiscard]] SpawnerStats& stats() { return _stats; }

private:
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
