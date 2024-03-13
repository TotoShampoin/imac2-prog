#pragma once

#include "prog/Boid.hpp"
#include <TotoGL/Primitives/Clock.hpp>
#include <TotoGL/RenderObject/Camera.hpp>
#include <TotoGL/RenderObject/RenderObject.hpp>
#include <TotoGL/Renderer.hpp>
#include <vector>

class BoidSpace {
public:
    BoidSpace(const size_t& amount);

    void update(const TotoGL::Seconds& delta);

    // Might tweak this later
    void render(TotoGL::Renderer& renderer, TotoGL::Camera& camera, TotoGL::RenderObject& object);
    void resize(const size_t& amount);
    void resetBoids(const std::optional<size_t>& amount = std::nullopt);

    const std::vector<Boid>& boids() { return _boids; }

    float avoidFactor() const { return _avoid_factor; };
    float matchingFactor() const { return _matching_factor; };
    float centeringFactor() const { return _centering_factor; };
    float cubeRadius() const { return _cube_radius; };

    float& avoidFactor() { return _avoid_factor; };
    float& matchingFactor() { return _matching_factor; };
    float& centeringFactor() { return _centering_factor; };
    float& cubeRadius() { return _cube_radius; };

private:
    std::vector<Boid> _boids;
    // TotoGL::RenderObjectInstanceId _object_instance;

    float _avoid_factor { .1 };
    float _matching_factor { .1 };
    float _centering_factor { .001 };
    float _cube_radius { 3 };
};
