#include "gfx/BoidRenderer.hpp"
#include "TotoGL/RenderObject/Camera.hpp"
#include "TotoGL/RenderObject/RenderObject.hpp"
#include "TotoGL/RenderObject/ShaderMaterial.hpp"
#include "TotoGL/Window.hpp"
#include "math/uniform.hpp"
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

    objects.cube_mesh->material().uniform("u_texture", objects.cube_texture);
    objects.cube_mesh->material().uniform("u_texture_blend", objects.cube_blend_texture);
    objects.cube_mesh->mesh().cull_face() = TotoGL::Mesh::CullFace::FRONT;

    objects.boid_mesh_low->scaling() = glm::vec3(.15);
    objects.boid_mesh_high->scaling() = glm::vec3(.15);
    objects.player_mesh->scaling() = glm::vec3(1.5);
    objects.bait_mesh->scaling() = glm::vec3(.15);

    objects.lights[DIRECTIONAL_LIGHT]->setDirection({ 1, -1, 1 });

    Random::Uniform<float> index_random { 0, static_cast<float>(objects.world_meshes.size()) };
    Random::Uniform<float> position_random { -25, 25 };
    for (int i = 0; i < 10; i++) {
        auto index = static_cast<size_t>(index_random());
        auto position = glm::vec3 { position_random(), position_random(), position_random() };
        while (
            glm::distance(position, { 0, 0, 0 }) < 15 || std::any_of(environment.begin(), environment.end(), [&](const auto& pair) {
                return glm::distance(pair.second, position) < 2;
            })) {
            position = glm::vec3 { position_random(), position_random(), position_random() };
        }
        environment.emplace_back(index, position);
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

// TODO(Rendering) Figure out why it's faster with the normal renderer than with the custom one
// even though the custom one is supposed to skip a lot of boids overhead...
void BoidRenderer::render(const BoidContainer& container, const Player& player, TotoGL::Camera& used_camera) {
    auto delta = clock.getDeltaTime();
    auto time = clock.getTime();

    objects.bound_mesh->scaling() = glm::vec3(static_cast<float>(container.cubeRadius() + .15f));
    objects.cube_mesh->scaling() = glm::vec3(static_cast<float>(container.cubeRadius() * 2.f));
    objects.player_mesh->position() = player.position();
    objects.lights[PLAYER_LIGHT]->position() = player.position() + player.direction() * .5f;
    if (glm::abs(player.direction().y) > .99) {
        objects.player_mesh->lookAt(player.position() + player.direction(), { 1, 0, 0 });
    } else {
        objects.player_mesh->lookAt(player.position() + player.direction());
    }

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

    for (const auto& [index, position] : environment) {
        objects.world_meshes[index]->position() = glm::rotate(glm::mat4(1), time / 10, { 0, 1, 0 }) * glm::vec4(position, 1);
        objects.world_meshes[index]->rotation() = glm::vec3(0, time, 0);
        renderer.render(*objects.world_meshes[index], used_camera, objects.lights);
    }

    // transparent objects here
    renderer.render(*objects.cube_mesh, used_camera, objects.lights);
};
