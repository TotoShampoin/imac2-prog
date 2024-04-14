#pragma once

#include "math/uniform.hpp"
#include "prog/Boid.hpp"
#include <glm/geometric.hpp>
#include <glm/glm.hpp>

class BoidSpawner {
public:
    void spawn(Boid& boid) {
        boid.position() = _position + _randomBox() * _radius;
        boid.velocity() = _randomBox() * _speed;
        boid.setParameters(_boid_force_parameters);
    }

private:
    glm::vec3 _randomBox() {
        return { _random_number_generator(), _random_number_generator(), _random_number_generator() };
    }

    glm::vec3 _position { 0 };
    glm::vec3 _direction { 0, 0, 1 };
    float _radius { .25 };
    float _speed { 1 };
    BoidForceParameters _boid_force_parameters {
        .avoid = { .force = .2, .zone_width = .5, .zone_offset = 0 },
        .match = { .force = .3, .zone_width = .5, .zone_offset = .5 },
        .center = { .force = .01, .zone_width = .5, .zone_offset = .5 }
    };
    Random::Uniform<float> _random_number_generator { -1, 1 };
};
