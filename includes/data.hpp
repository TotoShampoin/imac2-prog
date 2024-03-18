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

    int amount;
    bool spy;
    size_t spy_index;

    bool changing_amount = false;
    bool resetting = false;
    bool toggling_spy = false;
    bool spying_next = false;
    bool spying_previous = false;

    void update(const TotoGL::Seconds& delta);
    void draw();
};
