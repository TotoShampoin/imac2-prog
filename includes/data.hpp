#pragma once

#include "gfx/BoidScene.hpp"
#include "prog/BoidContainer.hpp"
#include <TotoGL/TotoGL.hpp>

constexpr auto WIDTH = 1024;
constexpr auto HEIGHT = 768;
constexpr auto FOV = glm::radians(70.);
constexpr auto NEAR = .1;
constexpr auto FAR = 100.;

struct Data {
    Data(TotoGL::Window&, TotoGL::Renderer&);
    ~Data() = default;

    void update(const TotoGL::Seconds& delta);
    void draw();

    int amount = 240;
    bool spy = false;
    size_t spy_index = 0;

    bool changing_amount = false;
    bool resetting = false;
    bool toggling_spy = false;
    bool spying_next = false;
    bool spying_previous = false;

    BoidContainer container { amount };
    BoidScene boid_scene;

    TotoGL::Window& window;
    TotoGL::Renderer& renderer;
    TotoGL::Clock clock;
    TotoGL::Scene scene;

    TotoGL::Camera camera { TotoGL::Camera::Perspective(FOV, (float)WIDTH / HEIGHT, NEAR, FAR) };
    TotoGL::OrbitControl orbit { -glm::pi<float>() / 6, glm::pi<float>() / 4, 10 };
};
