#pragma once

#include "TotoGL/Primitives/Texture.hpp"
#include "TotoGL/RenderObject/Light.hpp"
#include "TotoGL/RenderObject/MaterialObject.hpp"
#include "TotoGL/RenderObject/RenderObject.hpp"
#include "gfx/Player.hpp"
#include "prog/BoidContainer.hpp"
#include <TotoGL/TotoGL.hpp>
#include <glm/fwd.hpp>
#include <utility>
#include <vector>

constexpr auto WIDTH = 1280;
constexpr auto HEIGHT = 720;
constexpr auto FOV = glm::radians(70.);
constexpr auto NEAR = .1;
constexpr auto FAR = 100.;

constexpr auto FORCE_FIELD_SCROLL_PERIOD = 10.f;

struct RendererObjects {
    ~RendererObjects() {
        for (auto& light : lights) {
            TotoGL::LightFactory::destroy(light);
        }
        TotoGL::SkydomeFactory::destroy(skydome);
        TotoGL::TextureFactory::destroy(skydome_texture);
        TotoGL::TextureFactory::destroy(cube_texture);
        TotoGL::TextureFactory::destroy(cube_blend_texture);
        TotoGL::RenderObjectFactory::destroy(cube_mesh);
        TotoGL::MaterialObjectFactory::destroy(boid_mesh_high);
        TotoGL::MaterialObjectFactory::destroy(boid_mesh_low);
        TotoGL::MaterialObjectFactory::destroy(player_mesh);
        TotoGL::MaterialObjectFactory::destroy(bait_mesh);
        for (auto& world_mesh : world_meshes) {
            TotoGL::MaterialObjectFactory::destroy(world_mesh);
        }
    }
    RendererObjects() = default;
    RendererObjects(const RendererObjects&) = delete;
    RendererObjects(RendererObjects&&) = delete;
    RendererObjects& operator=(const RendererObjects&) = delete;
    RendererObjects& operator=(RendererObjects&&) = delete;

    std::vector<TotoGL::LightInstanceId> lights {
        TotoGL::LightFactory::create(TotoGL::Light({ .25, .5, 1 }, .1, TotoGL::LightType::AMBIENT)),
        TotoGL::LightFactory::create(TotoGL::Light({ 1, 1, 1 }, 1, TotoGL::LightType::DIRECTIONAL)),
        TotoGL::LightFactory::create(TotoGL::Light({ 1, .6, 0 }, 3, TotoGL::LightType::POINT))
    };

    TotoGL::TextureInstanceId skydome_texture {
        TotoGL::TextureFactory::create(TotoGL::Texture(std::ifstream("assets/textures/skydome.jpg")))
    };
    TotoGL::SkydomeInstanceId skydome {
        TotoGL::SkydomeFactory::create(TotoGL::Skydome(*skydome_texture))
    };

    TotoGL::TextureInstanceId cube_texture {
        TotoGL::TextureFactory::create(TotoGL::Texture(std::ifstream("assets/textures/noise.jpg")))
    };
    TotoGL::TextureInstanceId cube_blend_texture {
        TotoGL::TextureFactory::create(TotoGL::Texture(std::ifstream("assets/textures/noise_blend.png")))
    };
    TotoGL::RenderObjectInstanceId cube_mesh {
        TotoGL::RenderObjectFactory::create(
            TotoGL::RenderObject(
                TotoGL::MeshFactory::create(TotoGL::Mesh::cube()),
                TotoGL::ShaderMaterialFactory::create(
                    TotoGL::ShaderMaterial(
                        std::ifstream("assets/shaders/shader.vert"),
                        std::ifstream("assets/shaders/cube.frag")))))
    };

    TotoGL::MaterialObjectInstanceId boid_mesh_high {
        TotoGL::MaterialObjectFactory::create(
            TotoGL::loadWavefront("assets/models/rubio/high.obj"))
    };
    TotoGL::MaterialObjectInstanceId boid_mesh_low {
        TotoGL::MaterialObjectFactory::create(
            TotoGL::loadWavefront("assets/models/rubio/low.obj"))
    };

    TotoGL::MaterialObjectInstanceId player_mesh {
        TotoGL::MaterialObjectFactory::create(
            TotoGL::loadWavefront("assets/models/boyo/boyo.obj"))
    };

    TotoGL::MaterialObjectInstanceId bait_mesh {
        TotoGL::MaterialObjectFactory::create(
            TotoGL::loadWavefront("assets/models/gem.obj"))
    };

    std::vector<TotoGL::MaterialObjectInstanceId> world_meshes {
        TotoGL::MaterialObjectFactory::create(
            TotoGL::loadWavefront("assets/models/planet/planet.obj")),
        TotoGL::MaterialObjectFactory::create(
            TotoGL::loadWavefront("assets/models/planet/planet2.obj")),
        TotoGL::MaterialObjectFactory::create(
            TotoGL::loadWavefront("assets/models/planet/rock.obj")),
    };
};

struct BoidRenderer {
    BoidRenderer(TotoGL::Window&, TotoGL::Renderer&);
    ~BoidRenderer() = default;
    BoidRenderer(const BoidRenderer&) = delete;
    BoidRenderer(BoidRenderer&&) = delete;
    BoidRenderer& operator=(const BoidRenderer&) = delete;
    BoidRenderer& operator=(BoidRenderer&&) = delete;

    void render(const BoidContainer&, const Player&, TotoGL::Camera&);

    TotoGL::Renderer& renderer;
    TotoGL::Clock clock;

    RendererObjects objects;

    std::vector<std::pair<size_t, glm::vec3>> environment;
};
