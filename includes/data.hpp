#pragma once

#include "TotoGL/Primitives/BufferTexture.hpp"
#include "TotoGL/RenderObject/RenderObject.hpp"
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
    void draw(const TotoGL::Seconds& delta);

    int amount = 100;
    // bool spy = false;
    size_t spy_index = 0;

    bool changing_amount = false;
    bool resetting = false;
    bool toggling_spy = false;
    bool spying_next = false;
    bool spying_previous = false;

    BoidContainer container { amount };
    BoidRenderer boid_renderer;
    BoidScene boid_scene;

    TotoGL::Window& window;
    TotoGL::Renderer& renderer;
    TotoGL::Clock clock;
    TotoGL::Clock timer;

    std::vector<std::pair<std::string, float>> timers;

    TotoGL::Scene scene;
    TotoGL::RenderObjectInstanceId bounds {
        TotoGL::RenderObjectFactory::create(TotoGL::RenderObject(
            TotoGL::MeshFactory::create(TotoGL::Mesh::cube()),
            TotoGL::ShaderMaterialFactory::create(TotoGL::ShaderMaterial(
                std::ifstream("assets/shaders/shader.vert"),
                std::ifstream("assets/shaders/shader.frag")))))
    };
    TotoGL::LightInstanceId ambient_light {
        TotoGL::LightFactory::create(TotoGL::Light(
            glm::vec3(1, 1, 1), .1,
            TotoGL::LightType::AMBIENT))
    };
    TotoGL::LightInstanceId global_light {
        TotoGL::LightFactory::create(TotoGL::Light(
            glm::vec3(1, 1, 1), 1,
            TotoGL::LightType::DIRECTIONAL))
    };

    TotoGL::RenderObjectInstanceId monitor {
        TotoGL::RenderObjectFactory::create(TotoGL::RenderObject(
            TotoGL::MeshFactory::create(TotoGL::Mesh::quad()),
            TotoGL::ShaderMaterialFactory::create(TotoGL::ShaderMaterial(
                std::ifstream("assets/shaders/monitor.vert"),
                std::ifstream("assets/shaders/monitor.frag")))))
    };
    TotoGL::BufferTextureInstanceId monitor_texture {
        TotoGL::BufferTextureFactory::create(
            TotoGL::BufferTexture(512, 512))
    };
    TotoGL::Camera monitor_camera { TotoGL::Camera::Perspective(1, 1, .1, 100) };

    TotoGL::Camera camera { TotoGL::Camera::Perspective(FOV, (float)WIDTH / HEIGHT, NEAR, FAR) };
    TotoGL::OrbitControl orbit { -glm::pi<float>() / 6, glm::pi<float>() / 4, 10 };
};
