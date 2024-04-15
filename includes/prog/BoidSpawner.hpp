#pragma once

#include "TotoGL/Primitives/Color.hpp"
#include "math/enumerated.hpp"
#include "math/normal.hpp"
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
        boid.influence() = _randomGaussianCircle();
    }

    BoidForceParameters& boidForceParameters() { return _boid_force_parameters; }

    [[nodiscard]] glm::vec3 position() const { return _position; }
    [[nodiscard]] glm::vec3 direction() const { return _direction; }
    [[nodiscard]] float radius() const { return _radius; }
    [[nodiscard]] float speed() const { return _speed; }

private:
    glm::vec2 _randomSquare() {
        return { _uniform_generator(), _uniform_generator() };
    }
    glm::vec3 _randomBox() {
        return { _uniform_generator(), _uniform_generator(), _uniform_generator() };
    }
    glm::vec3 _randomGaussianBox() {
        return { _gaussian_generator(), _gaussian_generator(), _gaussian_generator() };
    }
    glm::vec3 _randomSphere() {
        return glm::normalize(_randomGaussianBox());
    }
    glm::vec2 _randomGaussianCircle() {
        auto [a, b] = _gaussian_generator.generatePair();
        return { a, b };
    }

    glm::vec3 _position { 0 };
    glm::vec3 _direction { 0, 0, 1 };
    float _radius { .05 };
    float _speed { 1 };
    BoidForceParameters _boid_force_parameters {
        .avoid = { .force = .2, .zone_width = .5, .zone_offset = 0 },
        .match = { .force = .3, .zone_width = .5, .zone_offset = .5 },
        .center = { .force = .01, .zone_width = .5, .zone_offset = .5 }
    };

    Random::Uniform<float> _uniform_generator { -1, 1 };
    Random::Normal<float> _gaussian_generator { 0, 1 };
    Random::Enumerated<TotoGL::ColorRGB> _color_generator { {
        { .75f, YELLOW },
        { .05f, GREEN },
        { .05f, RED },
        { .05f, BLUE },
        { .05f, MAGENTA },
        { .05f, CYAN },
    } };
};
