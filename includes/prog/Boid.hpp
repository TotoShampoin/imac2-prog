#pragma once

#include "TotoGL/Primitives/Clock.hpp"
#include "TotoGL/Primitives/Color.hpp"
#include "prog/Bait.hpp"
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
    BoidForce bias;
};

struct BoidSpeedCaps {
    float min { 0 };
    float max { 0 };
};

class Boid {
public:
    explicit Boid(
        const BoidCoordinates& = {},
        const BoidForceParameters& = {},
        const BoidSpeedCaps& = {},
        const TotoGL::ColorRGB& = { 1, 1, 0 });

    [[nodiscard]] float closeness(const Boid&, const BoidForce&) const;

    [[nodiscard]] glm::vec3 separation(const std::vector<Boid>&, const BoidForce&) const;
    [[nodiscard]] glm::vec3 alignment(const std::vector<Boid>&, const BoidForce&) const;
    [[nodiscard]] glm::vec3 cohesion(const std::vector<Boid>&, const BoidForce&) const;
    [[nodiscard]] glm::vec3 bias(const std::vector<Bait>&, const BoidForce&) const;
    [[nodiscard]] glm::vec3 bias(const glm::vec3&) const;

    // void updateVelocity(const std::vector<Boid>&);
    void updatePosition(const TotoGL::Seconds&);
    void setParameters(const BoidForceParameters&);
    void setSpeedCaps(const BoidSpeedCaps&);

    [[nodiscard]] glm::vec3 position() const { return _position; }
    [[nodiscard]] glm::vec3 velocity() const { return _velocity; }
    [[nodiscard]] BoidForce avoidForce() const { return _avoid_force; }
    [[nodiscard]] BoidForce matchForce() const { return _match_force; }
    [[nodiscard]] BoidForce centerForce() const { return _center_force; }
    [[nodiscard]] BoidForce biasForce() const { return _bias_force; }
    [[nodiscard]] float minSpeed() const { return _min_speed; }
    [[nodiscard]] float maxSpeed() const { return _max_speed; }
    [[nodiscard]] TotoGL::ColorRGB color() const { return _color; }
    [[nodiscard]] bool isAlive() const { return _is_alive; }
    [[nodiscard]] TotoGL::Seconds lifetime() const { return _lifetime; }

    glm::vec3& position() { return _position; }
    glm::vec3& velocity() { return _velocity; }
    BoidForce& avoidForce() { return _avoid_force; }
    BoidForce& matchForce() { return _match_force; }
    BoidForce& centerForce() { return _center_force; }
    BoidForce& biasForce() { return _bias_force; }
    float& minSpeed() { return _min_speed; }
    float& maxSpeed() { return _max_speed; }
    TotoGL::ColorRGB& color() { return _color; }
    bool& isAlive() { return _is_alive; }
    TotoGL::Seconds& lifetime() { return _lifetime; }

private:
    glm::vec3 _position;
    glm::vec3 _velocity;
    BoidForce _avoid_force;
    BoidForce _match_force;
    BoidForce _center_force;
    BoidForce _bias_force;
    float _min_speed;
    float _max_speed;
    TotoGL::ColorRGB _color;
    bool _is_alive;
    TotoGL::Seconds _lifetime;
};
