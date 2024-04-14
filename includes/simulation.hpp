#pragma once

#include "TotoGL/RenderObject/Camera.hpp"
#include "TotoGL/Window.hpp"
#include "gfx/BoidRenderer.hpp"
#include "gfx/Gui.hpp"
#include "gfx/Player.hpp"
#include "prog/BoidContainer.hpp"
#include "prog/BoidSpawner.hpp"
#include <vector>

class Simulation {
public:
    Simulation(TotoGL::Window&, TotoGL::Renderer&);
    ~Simulation() = default;
    Simulation() = delete;
    Simulation(const Simulation&) = delete;
    Simulation(Simulation&&) = delete;
    Simulation& operator=(const Simulation&) = delete;
    Simulation& operator=(Simulation&&) = delete;

    void update(const TotoGL::Seconds& delta);
    void draw(const TotoGL::Seconds& delta);

    TotoGL::Seconds getDeltaTime() { return clock.getDeltaTime(); }

private:
    // int amount = 100;
    // bool spy = false;
    // size_t spy_index = 0;

    TotoGL::Window& window;

    BoidRenderer boid_renderer;
    UiRenderer ui_renderer;
    UiVariables ui_variables;

    BoidSpawner spawner;
    BoidContainer container { ui_variables.amount, [&](Boid& boid) { spawner.spawn(boid); } };
    Player player;
    glm::vec3 player_direction { 0 };

    TotoGL::Clock clock;
    TotoGL::Clock timer;

    TotoGL::BufferTextureInstanceId monitor_texture { TotoGL::BufferTextureFactory::create(TotoGL::BufferTexture(256, 192)) };
    TotoGL::Camera camera { TotoGL::Camera::Perspective(glm::radians(60.f), static_cast<float>(WIDTH) / HEIGHT, .1, 100) };
    TotoGL::Camera monitor_camera { TotoGL::Camera::Perspective(glm::radians(90.f), static_cast<float>(256) / 192, .15, 100) };
    TotoGL::OrbitControl orbit { 0, -glm::pi<float>() / 12, 5 };

    std::vector<std::pair<std::string, float>> timers;
};
