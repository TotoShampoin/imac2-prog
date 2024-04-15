#pragma once

#include "TotoGL/Primitives/Color.hpp"
#include "math/enumerated.hpp"
#include "math/uniform.hpp"
#include "prog/Boid.hpp"
#include <glm/geometric.hpp>
#include <glm/glm.hpp>

constexpr TotoGL::ColorRGB BLACK { 0, 0, 0 };
constexpr TotoGL::ColorRGB RED { 1, 0, 0 };
constexpr TotoGL::ColorRGB GREEN { 0, 1, 0 };
constexpr TotoGL::ColorRGB YELLOW { 1, 1, 0 };
constexpr TotoGL::ColorRGB BLUE { 0, 0, 1 };
constexpr TotoGL::ColorRGB MAGENTA { 1, 0, 1 };
constexpr TotoGL::ColorRGB CYAN { 0, 1, 1 };
constexpr TotoGL::ColorRGB WHITE { 1, 1, 1 };

class BoidSpawner {
public:
    void spawn(Boid& boid) {
        boid.position() = _position + _randomBox() * _radius;
        boid.velocity() = _randomBox() * _speed;
        boid.setParameters(_boid_force_parameters);
        boid.color() = _color_generator();
    }

    BoidForceParameters& boidForceParameters() { return _boid_force_parameters; }

    [[nodiscard]] glm::vec3 position() const { return _position; }
    [[nodiscard]] glm::vec3 direction() const { return _direction; }
    [[nodiscard]] float radius() const { return _radius; }
    [[nodiscard]] float speed() const { return _speed; }

private:
    glm::vec3 _randomBox() {
        return { _coordinate_generator(), _coordinate_generator(), _coordinate_generator() };
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
    Random::Uniform<float> _coordinate_generator { -1, 1 };
    Random::Enumerated<TotoGL::ColorRGB> _color_generator { {
        { .5f, YELLOW },
        { .1f, GREEN },
        { .1f, RED },
        { .1f, BLUE },
        { .1f, MAGENTA },
        { .1f, CYAN },
    } };
};
