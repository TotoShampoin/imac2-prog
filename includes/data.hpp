#pragma once

#include "prog/BoidSpace.hpp"
#include <TotoGL/TotoGL.hpp>

constexpr auto WIDTH = 1024;
constexpr auto HEIGHT = 768;
constexpr auto FOV = glm::radians(70.);
constexpr auto NEAR = .1;
constexpr auto FAR = 100.;

struct Data {
    Data(TotoGL::Window&);
    ~Data();

    TotoGL::Window& window;
    TotoGL::Renderer renderer;
    TotoGL::Clock clock;

    TotoGL::Camera camera;
    TotoGL::OrbitControl orbit;

    TotoGL::RenderObjectInstanceId appearance;
    BoidSpace boid_space;

    bool spy = false;
    size_t spy_index = 0;

    void update(const TotoGL::Seconds& delta);
    void draw();
};
