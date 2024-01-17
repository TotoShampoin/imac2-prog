#include <TotoGL.hpp>
#include <chrono>
#include <memory>

int main(int, const char *[]) {
    auto window = TotoGL::Window::create(640, 480, "Boids");
    auto renderer = TotoGL::GL3Renderer::create(window);

    auto scene = TotoGL::Scene::create();
    auto camera = TotoGL::PerspectiveCamera::create(45, 4 / 3., 1, 100);

    auto geometry = TotoGL::BoxGeometry::create();
    // auto geometry = TotoGL::SphereGeometry::create();
    auto material = TotoGL::PhongMaterial::create();
    material->diffuse(TotoGL::ColorRGB(1, .5, 0));
    material->specular(TotoGL::ColorRGB(1, 1, 1));
    material->shininess(4);

    auto mesh = TotoGL::Mesh::create(geometry, material);

    auto light = TotoGL::PointLight::create();
    light->position() = { 2, 3, 1 };
    light->strength() = 5;

    scene->add(mesh);
    scene->add(light);

    camera->position() = { 2, 2, 2 };
    camera->lookAt({ 0, 0, 0 });

    auto clock = TotoGL::Clock::create();
    while (!window->shouldClose()) {
        const float delta_time = clock->delta_time().count() / 1000.;
        mesh->rotation().x() += delta_time;
        mesh->rotation().y() += delta_time;
        mesh->rotation().z() += delta_time;

        window->callBeforeSwap([&]() {
            renderer->render(scene, camera);
            //
        });

        window->pollEvents();
    }

    return 0;
}
