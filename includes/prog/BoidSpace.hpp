#pragma once

#include "prog/Boid.hpp"
#include <TotoGL/Primitives/Clock.hpp>
#include <TotoGL/RenderObject/Camera.hpp>
#include <TotoGL/RenderObject/RenderObject.hpp>
#include <TotoGL/Renderer.hpp>
#include <vector>

class BoidSpace {
public:
    BoidSpace(const size_t& amount, const TotoGL::RenderObjectFactory::ObjectInstanceId& object);

    void update(const TotoGL::Seconds& delta);

    // Might tweak this later
    void render(TotoGL::Renderer& renderer, TotoGL::Camera& camera);

    const std::vector<Boid>& boids() { return _boids; }

private:
    std::vector<Boid> _boids;
    TotoGL::RenderObjectFactory::ObjectInstanceId _object_instance;
};
