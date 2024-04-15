#pragma once

#include "TotoGL/Primitives/Clock.hpp"
#include "TotoGL/Primitives/Color.hpp"
#include <glm/glm.hpp>
#include <vector>

// https://vanhunteradams.com/Pico/Animal_Movement/Boids-algorithm.html

struct BoidCoordinates {
    glm::vec3 position { 0, 0, 0 };
    glm::vec3 velocity { 0, 0, 1 };
};

struct BoidForce {
    float force { 0 };
    float zone_width { 0 };
    float zone_offset { 0 };
};

struct BoidForceParameters {
    BoidForce avoid;
    BoidForce match;
    BoidForce center;
};

class Boid {
public:
    explicit Boid(
        const BoidCoordinates& = {},
        const BoidForceParameters& = {},
        const TotoGL::ColorRGB& = { 1, 1, 0 });

    [[nodiscard]] float closeness(const Boid&, const BoidForce&) const;

    [[nodiscard]] glm::vec3 separation(const std::vector<Boid>&, const BoidForce&) const;
    [[nodiscard]] glm::vec3 alignment(const std::vector<Boid>&, const BoidForce&) const;
    [[nodiscard]] glm::vec3 cohesion(const std::vector<Boid>&, const BoidForce&) const;

    // void updateVelocity(const std::vector<Boid>&);
    void updatePosition(const TotoGL::Seconds&);
    void setParameters(const BoidForceParameters&);

    [[nodiscard]] glm::vec3 position() const { return _position; }
    [[nodiscard]] glm::vec3 velocity() const { return _velocity; }
    [[nodiscard]] BoidForce avoidForce() const { return _avoid_force; }
    [[nodiscard]] BoidForce matchForce() const { return _match_force; }
    [[nodiscard]] BoidForce centerForce() const { return _center_force; }
    [[nodiscard]] TotoGL::ColorRGB color() const { return _color; }
    [[nodiscard]] glm::vec2 influence() const { return _influence; }

    glm::vec3& position() { return _position; }
    glm::vec3& velocity() { return _velocity; }
    BoidForce& avoidForce() { return _avoid_force; }
    BoidForce& matchForce() { return _match_force; }
    BoidForce& centerForce() { return _center_force; }
    TotoGL::ColorRGB& color() { return _color; }
    glm::vec2& influence() { return _influence; }

private:
    glm::vec3 _position;
    glm::vec3 _velocity;
    BoidForce _avoid_force;
    BoidForce _match_force;
    BoidForce _center_force;
    TotoGL::ColorRGB _color;
    glm::vec2 _influence;
};
