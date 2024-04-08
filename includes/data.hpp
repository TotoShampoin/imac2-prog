#pragma once

#include "TotoGL/RenderObject/Camera.hpp"
#include "TotoGL/Window.hpp"
#include "gfx/BoidRenderer.hpp"
#include "prog/BoidContainer.hpp"
#include <vector>

struct Data {
    Data(TotoGL::Window&, TotoGL::Renderer&);
    ~Data() = default;

    void update(const TotoGL::Seconds& delta);
    void draw(const TotoGL::Seconds& delta);

    int amount = 100;
    // bool spy = false;
    size_t spy_index = 0;

    bool changing_amount = false;
    bool resetting = false;
    bool toggling_spy = false;
    bool spying_next = false;
    bool spying_previous = false;

    TotoGL::Window& window;

    BoidContainer container { amount };
    BoidRenderer boid_renderer;

    TotoGL::Clock clock;
    TotoGL::Clock timer;

    TotoGL::BufferTextureInstanceId monitor_texture;
    TotoGL::Camera monitor_camera { TotoGL::Camera::Perspective(1, (float)256 / 192, .1, 100) };

    std::vector<std::pair<std::string, float>> timers;
};
