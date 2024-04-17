#pragma once

#include "TotoGL/Primitives/Color.hpp"
#include "math/enumerated.hpp"
#include "math/normal.hpp"
#include "math/uniform.hpp"
#include "prog/Bait.hpp"
#include "prog/Boid.hpp"
#include <glm/ext/matrix_transform.hpp>
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
    void spawnBoid(Boid& boid) {
        auto random_position = glm::vec3(_position_generator(), _position_generator(), _position_generator());
        auto random_angles = _getRandom2D(_angle_generator);
        auto color = _color_generator();

        auto rotated = glm::rotate(glm::mat4(1), random_angles.x, _up()) * glm::rotate(glm::mat4(1), random_angles.y, _right());
        BoidForceParameters parameters = _boid_force_parameters;
        parameters.avoid.force += _strength_generator();
        parameters.center.force += _strength_generator();
        parameters.match.force += _strength_generator();
        parameters.bias.force += _strength_generator();

        boid.position() = _position + random_position * _position_radius;
        boid.velocity() = (rotated * glm::vec4(_direction, 1)) * _boid_speed;
        boid.setParameters(parameters);
        boid.color() = color;
    }

    void spawnBait(Bait& bait) {
        auto random_position = glm::vec3(_position_generator(), _position_generator(), _position_generator());
        bait.position() = _position + random_position * _position_radius;
    }

    [[nodiscard]] glm::vec3& position() { return _position; }
    [[nodiscard]] glm::vec3& direction() { return _direction; }
    [[nodiscard]] float& positionRadius() { return _position_radius; }
    [[nodiscard]] float& boidSpeed() { return _boid_speed; }
    [[nodiscard]] BoidForceParameters& boidForceParameters() { return _boid_force_parameters; }

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
    float _boid_speed { 1 };
    BoidForceParameters _boid_force_parameters {
        .avoid = { .force = .2, .zone_width = .5, .zone_offset = 0 },
        .match = { .force = .3, .zone_width = .5, .zone_offset = .5 },
        .center = { .force = .01, .zone_width = .5, .zone_offset = .5 },
        .bias = { .force = .25, .zone_width = 1.5, .zone_offset = 0 },
    };

    Random::Uniform<float> _position_generator { -1, 1 };
    Random::Normal<float> _angle_generator { 0, .25 };
    Random::Normal<float> _strength_generator { 0, .05 };
    Random::Enumerated<TotoGL::ColorRGB> _color_generator { {
        { .75f, YELLOW },
        { .05f, GREEN },
        { .05f, RED },
        { .05f, BLUE },
        { .05f, MAGENTA },
        { .05f, CYAN },
    } };
};
