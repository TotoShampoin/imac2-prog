#include "gfx/BoidRenderer.hpp"
#include "TotoGL/RenderObject/Camera.hpp"
#include "TotoGL/Window.hpp"
#include "prog/imgui-impl.hpp"

constexpr size_t AMBIENT_LIGHT = 0;
constexpr size_t DIRECTIONAL_LIGHT = 1;

BoidRenderer::BoidRenderer(TotoGL::Window& window, TotoGL::Renderer& renderer)
    : renderer(renderer) {

    skydome_texture = TotoGL::TextureFactory::create(
        TotoGL::Texture(std::ifstream("assets/textures/skydome.jpg")));
    bound_mesh = TotoGL::MaterialObjectFactory::create(
        TotoGL::loadWavefront("assets/models/aquarium.obj"));
    boid_mesh_high = TotoGL::MaterialObjectFactory::create(
        TotoGL::loadWavefront("assets/models/rubio/high.obj"));
    boid_mesh_low = TotoGL::MaterialObjectFactory::create(
        TotoGL::loadWavefront("assets/models/rubio/low.obj"));

    camera = TotoGL::Camera::Perspective(FOV, (float)WIDTH / HEIGHT, NEAR, FAR);
    lights.push_back(TotoGL::LightFactory::create(TotoGL::Light({ 1, 1, 1 }, .1, TotoGL::LightType::AMBIENT)));
    lights.push_back(TotoGL::LightFactory::create(TotoGL::Light({ 1, 1, 1 }, 1, TotoGL::LightType::DIRECTIONAL)));
    skydome = TotoGL::SkydomeFactory::create(TotoGL::Skydome(*skydome_texture));

    window.on(TotoGL::VectorEventName::WINDOW_SIZE, [&](const TotoGL::VectorEvent& event) {
        glViewport(0, 0, event.x, event.y);
        camera.setPersective(FOV, event.x / event.y, NEAR, FAR);
    });
    orbit.bindEvents(window, isImGuiFocused);

    boid_mesh_low->scaling() = glm::vec3(.15);
    boid_mesh_high->scaling() = glm::vec3(.15);

    lights[DIRECTIONAL_LIGHT]->setDirection({ 1, -1, 1 });
}

BoidRenderer::~BoidRenderer() {
    TotoGL::SkydomeFactory::destroy(skydome);
    TotoGL::LightFactory::destroy(lights[DIRECTIONAL_LIGHT]);
    TotoGL::LightFactory::destroy(lights[AMBIENT_LIGHT]);
    TotoGL::MaterialObjectFactory::destroy(boid_mesh_low);
    TotoGL::MaterialObjectFactory::destroy(boid_mesh_high);
    TotoGL::MaterialObjectFactory::destroy(bound_mesh);
    TotoGL::TextureFactory::destroy(skydome_texture);
}

void BoidRenderer::update(const BoidContainer& container) {
    bound_mesh->scaling() = { container.cubeRadius() + .15, container.cubeRadius() + .15, container.cubeRadius() + .15 };
    orbit.apply(camera);
}

// TODO(Rendering) Figure out why it's faster with the normal renderer than with the custom one
// even though the custom one is supposed to skip a lot of boids overhead...
void BoidRenderer::render(const BoidContainer& container, std::optional<TotoGL::Camera> camera_opt) {
    auto& used_camera = camera_opt.has_value() ? camera_opt.value() : this->camera;
    renderer.clear();
    renderer.render(skydome->object(), used_camera);
    renderer.clear(false, true, false);
    renderer.render(*bound_mesh, used_camera, lights);
    for (auto& boid : container.boids()) {
        if (glm::distance(boid.position(), used_camera.position()) < 5) {
            boid_mesh_high->position() = boid.position();
            boid_mesh_high->lookAt(boid.position() + boid.velocity());
            renderer.render(*boid_mesh_high, used_camera, lights);
        } else {
            boid_mesh_low->position() = boid.position();
            boid_mesh_low->lookAt(boid.position() + boid.velocity());
            renderer.render(*boid_mesh_low, used_camera, lights);
        }
    }
};
