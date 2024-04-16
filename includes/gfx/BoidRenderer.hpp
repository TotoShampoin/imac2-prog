#pragma once

#include "TotoGL/Primitives/Texture.hpp"
#include "TotoGL/RenderObject/Light.hpp"
#include "gfx/Player.hpp"
#include "prog/BoidContainer.hpp"
#include <TotoGL/TotoGL.hpp>
#include <vector>

constexpr auto WIDTH = 1280;
constexpr auto HEIGHT = 720;
constexpr auto FOV = glm::radians(70.);
constexpr auto NEAR = .1;
constexpr auto FAR = 100.;

struct BoidRenderer {
    BoidRenderer(TotoGL::Window&, TotoGL::Renderer&);
    ~BoidRenderer();
    BoidRenderer(const BoidRenderer&) = delete;
    BoidRenderer(BoidRenderer&&) = delete;
    BoidRenderer& operator=(const BoidRenderer&) = delete;
    BoidRenderer& operator=(BoidRenderer&&) = delete;

    void render(const BoidContainer&, const Player&, TotoGL::Camera&);

    TotoGL::Renderer& renderer;

    TotoGL::TextureInstanceId skydome_texture;
    TotoGL::MaterialObjectInstanceId bound_mesh;
    TotoGL::MaterialObjectInstanceId boid_mesh_high;
    TotoGL::MaterialObjectInstanceId boid_mesh_low;
    TotoGL::MaterialObjectInstanceId player_mesh;

    std::vector<TotoGL::LightInstanceId> lights;
    TotoGL::SkydomeInstanceId skydome;
};
