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
    ~RendererObjects();
    RendererObjects();
    RendererObjects(const RendererObjects&) = delete;
    RendererObjects(RendererObjects&&) = delete;
    RendererObjects& operator=(const RendererObjects&) = delete;
    RendererObjects& operator=(RendererObjects&&) = delete;

    std::vector<TotoGL::LightInstanceId> lights;

    TotoGL::TextureInstanceId skydome_texture;
    TotoGL::SkydomeInstanceId skydome;

    TotoGL::TextureInstanceId cube_texture;
    TotoGL::TextureInstanceId cube_blend_texture;
    TotoGL::RenderObjectInstanceId cube_mesh;

    TotoGL::MaterialObjectInstanceId boid_mesh_high;
    TotoGL::MaterialObjectInstanceId boid_mesh_low;

    TotoGL::MaterialObjectInstanceId player_mesh;

    TotoGL::MaterialObjectInstanceId bait_mesh;

    std::vector<TotoGL::MaterialObjectInstanceId> world_meshes;
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
