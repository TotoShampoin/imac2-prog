#pragma once

#include "TotoGL/Primitives/Texture.hpp"
#include "TotoGL/RenderObject/Light.hpp"
#include "gfx/Player.hpp"
#include "prog/BoidContainer.hpp"
#include <TotoGL/TotoGL.hpp>
#include <vector>

constexpr auto WIDTH = 1024;
constexpr auto HEIGHT = 768;
constexpr auto FOV = glm::radians(70.);
constexpr auto NEAR = .1;
constexpr auto FAR = 100.;

struct BoidRenderer {
    BoidRenderer(TotoGL::Window&, TotoGL::Renderer&);
    ~BoidRenderer();

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
