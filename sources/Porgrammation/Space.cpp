#include "Programmation/Space.hpp"
#include <glm/geometric.hpp>
#include <memory>
#include <random>

Space::Space(const size_t &nb_boids)
    : _scene(TotoGL::Scene::create()),
      _camera(TotoGL::PerspectiveCamera::create(45, 4 / 3., 1, 100)),
      _boid_mesh(TotoGL::Mesh::create(
          TotoGL::BoxGeometry::create(),
          TotoGL::PhongMaterial::create(
              TotoGL::ColorRGB(1, .5, 0),
              TotoGL::ColorRGB(1, 1, 1),
              32.))),
      _boids(nb_boids) {
    static auto distribution = std::uniform_real_distribution<float>(-1, 1);
    static auto rng = std::default_random_engine();
    for (auto &boid : _boids) {
        boid.position().x = distribution(rng);
        boid.position().y = distribution(rng);
        boid.position().z = distribution(rng);
    }
}

void Space::update(const TotoGL::Seconds &time) {
    for (auto &boid : _boids) {
        boid.updateVelocity(_boids);
        // if (glm::length(boid.velocity()) > 1) {
        //     boid.velocity() = glm::normalize(boid.velocity()) * 1.f;
        // }
    }
    clearScene();
    for (auto &boid : _boids) {
        boid.updatePosition(time);
        addToScene(boid);
    }
}

void Space::display(const TotoGL::GL3RendererPtr &renderer) {
    renderer->window()->callBeforeSwap([&]() {
        renderer->render(_scene, _camera);
    });
    renderer->window()->pollEvents();
}

void Space::clearScene() {
    // TODO: Light color initializzation
    static const auto ambient = []() {
        const auto light = TotoGL::AmbientLight::create();
        light->color() = TotoGL::ColorRGB(1.);
        light->strength() = .1;
        return light;
    }();
    static const auto dirlight = []() {
        const auto light = TotoGL::DirectionalLight::create();
        light->color() = TotoGL::ColorRGB(1.);
        light->strength() = 2.;
        light->direction() = TotoGL::Vector3(-2, -3, -1);
        return light;
    }();
    _scene->clear();
    _scene->add(ambient);
    _scene->add(dirlight);
}
void Space::addToScene(const Boid &boid) {
    // TODO: ObjectPtr Object::clone(bool recursive)
    const auto cloned_mesh = TotoGL::Mesh::create(
        _boid_mesh->geometry(),
        _boid_mesh->material());
    cloned_mesh->position() = boid.position();
    cloned_mesh->scale() = TotoGL::Vector3(.1);
    cloned_mesh->lookAt(boid.position() + boid.velocity());
    _scene->add(cloned_mesh);
}
