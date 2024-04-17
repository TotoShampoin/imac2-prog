#pragma once

#include "prog/Bait.hpp"
#include "prog/Boid.hpp"
#include <functional>
#include <vector>

class BoidContainer {
public:
    explicit BoidContainer(
        const size_t& amount,
        const std::function<void(Boid&)>& = [](Boid&) {});

    void update(const TotoGL::Seconds& delta);

    void resize(
        const size_t& amount,
        const std::function<void(Boid&)>& = [](Boid&) {});
    void resetBoids(
        const std::optional<size_t>& amount = std::nullopt,
        const std::function<void(Boid&)>& = [](Boid&) {});

    void addBoids(
        const size_t& = 1,
        const std::function<void(Boid&)>& = [](Boid&) {});
    void destroyBoids(const size_t& = 1);

    void addBait(const Bait&);
    void destroyBaits();

    [[nodiscard]] const std::vector<Boid>& boids() const { return _boids; }
    [[nodiscard]] const std::vector<Bait>& baits() const { return _baits; }

    [[nodiscard]] float cubeRadius() const { return _cube_radius; };
    [[nodiscard]] float minVelocity() const { return _min_velocity; };
    [[nodiscard]] float maxVelocity() const { return _max_velocity; };
    [[nodiscard]] const BoidForce& cubeForce() const { return _cube_force; };
    [[nodiscard]] const Boid& playerBoid() const { return _player_boid; };

    float& cubeRadius() { return _cube_radius; };
    float& minVelocity() { return _min_velocity; };
    float& maxVelocity() { return _max_velocity; };
    BoidForce& cubeForce() { return _cube_force; };
    Boid& playerBoid() { return _player_boid; };

    void updateCubeBoids(const glm::vec3& center);
    void updatePlayerBoid(const glm::vec3& position);

private:
    std::vector<Boid> _boids;
    std::vector<Bait> _baits;
    std::vector<Boid> _cube_boids { 6 };
    Boid _player_boid {};

    BoidForce _cube_force { .force = 10, .zone_width = .3, .zone_offset = 0 };
    BoidForce _player_force { .force = 5, .zone_width = 1, .zone_offset = 0 };

    float _cube_radius { 10 };
    float _min_velocity { 1 };
    float _max_velocity { 2 };
};
