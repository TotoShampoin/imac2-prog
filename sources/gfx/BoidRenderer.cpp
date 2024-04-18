#include "gfx/BoidRenderer.hpp"
#include "TotoGL/RenderObject/Camera.hpp"
#include "TotoGL/RenderObject/RenderObject.hpp"
#include "TotoGL/RenderObject/ShaderMaterial.hpp"
#include "TotoGL/Window.hpp"
#include "math/random/uniform.hpp"
#include "math/variables.hpp"
#include <algorithm>

constexpr size_t AMBIENT_LIGHT = 0;
constexpr size_t DIRECTIONAL_LIGHT = 1;
constexpr size_t PLAYER_LIGHT = 2;

constexpr size_t BOID_MESH_LOW_MATERIAL = 0;
constexpr size_t BOID_MESH_HIGH_EYE_MATERIAL = 2;

BoidRenderer::BoidRenderer(TotoGL::Window& window, TotoGL::Renderer& renderer)
    : renderer(renderer) {
    auto [width, height] = window.size();
    if (width == 0 || height == 0) {
        width = WIDTH;
        height = HEIGHT;
    }

    objects.cube_mesh->material().uniform("u_color", glm::vec3(1, 0, 1));
    objects.cube_mesh->material().uniform("u_texture", objects.cube_texture);
    objects.cube_mesh->material().uniform("u_texture_blend", objects.cube_blend_texture);
    objects.cube_mesh->mesh().cull_face() = TotoGL::Mesh::CullFace::FRONT;

    objects.boid_mesh_low->scaling() = glm::vec3(.15);
    objects.boid_mesh_high->scaling() = glm::vec3(.15);
    objects.player_mesh->scaling() = glm::vec3(1.5);
    objects.bait_mesh->scaling() = glm::vec3(.15);

    objects.lights[DIRECTIONAL_LIGHT]->setDirection({ 1, -1, 1 });

    regeneratePlanets(10);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

// TODO(Rendering) Figure out why it's faster with the normal renderer than with the custom one
// even though the custom one is supposed to skip a lot of boids overhead...
void BoidRenderer::render(const BoidContainer& container, const Player& player, TotoGL::Camera& used_camera) {
    auto delta = clock.getDeltaTime();
    auto time = clock.getTime();

    objects.cube_mesh->scaling() = glm::vec3(static_cast<float>(container.cubeRadius() * 2.f));

    objects.player_mesh->position() = player.position();
    if (glm::abs(player.direction().y) < .99) {
        objects.player_mesh->lookAt(player.position() + player.direction());
    } else {
        objects.player_mesh->lookAt(player.position() + player.direction(), player.frontDirection());
    }
    objects.lights[PLAYER_LIGHT]->position() = objects.player_mesh->position() + objects.player_mesh->transformation().front() * .1f * objects.player_mesh->scaling().z;

    objects.cube_mesh->material().uniform("u_time", time / FORCE_FIELD_SCROLL_PERIOD);

    renderer.clear();
    renderer.render(objects.skydome->object(), used_camera);
    renderer.clear(false, true, false);

    renderer.render(*objects.player_mesh, used_camera, objects.lights);
    for (const auto& boid : container.boids()) {
        if (glm::distance(boid.position(), used_camera.position()) < 5) {
            objects.boid_mesh_high->materials()[BOID_MESH_HIGH_EYE_MATERIAL].emissive = boid.color();
            objects.boid_mesh_high->position() = boid.position();
            objects.boid_mesh_high->lookAt(boid.position() + boid.velocity());
            renderer.render(*objects.boid_mesh_high, used_camera, objects.lights);
        } else {
            objects.boid_mesh_low->materials()[BOID_MESH_LOW_MATERIAL].emissive = boid.color();
            objects.boid_mesh_low->position() = boid.position();
            objects.boid_mesh_low->lookAt(boid.position() + boid.velocity());
            renderer.render(*objects.boid_mesh_low, used_camera, objects.lights);
        }
    }
    objects.bait_mesh->rotation() += glm::vec3(2, 3, 5) * delta;
    for (const auto& bait : container.baits()) {
        objects.bait_mesh->position() = bait.position();
        renderer.render(*objects.bait_mesh, used_camera, objects.lights);
    }

    for (const auto& [mesh, position, orbit_cycle, daylight_cycle] : environment_meshes) {
        mesh->position() = glm::rotate(glm::mat4(1), time * daylight_cycle / 10, { 0, 1, 0 }) * glm::vec4(position, 1);
        mesh->rotation() = glm::vec3(0, time * orbit_cycle, 0);
        renderer.render(*mesh, used_camera, objects.lights);
    }

    // transparent objects here
    renderer.render(*objects.cube_mesh, used_camera, objects.lights);
};

void BoidRenderer::regeneratePlanets(const size_t& amount) {
    auto& index_random = Variables::instance()._renderer_index_random;
    auto& position_random = Variables::instance()._renderer_position_random;
    auto& orbit_random = Variables::instance()._renderer_orbit_random;
    // auto& daylight_random = Variables::instance()._renderer_daylight_random;

    for (int i = 0; i < amount; i++) {
        auto index = static_cast<size_t>(index_random() * static_cast<float>(objects.world_meshes.size()));
        auto mesh = objects.world_meshes[index];
        auto position = glm::vec3 { position_random(), position_random(), position_random() };
        while (
            glm::distance(position, { 0, 0, 0 }) < 15 || std::any_of(environment_meshes.begin(), environment_meshes.end(), [&](const auto& env_mesh) {
                return glm::distance(env_mesh.position, position) < 2;
            })) {
            position = glm::vec3 { position_random(), position_random(), position_random() };
        }
        environment_meshes.emplace_back(mesh, position, orbit_random(), orbit_random());
    }
}
