#include "prog/BoidSpace.hpp"
#include <algorithm>
#include <random>

BoidSpace::BoidSpace(const size_t& amount, const TotoGL::RenderObjectFactory::ObjectInstanceId& object)
    : _boids(amount)
    , _object_instance(object) {
    auto distribution = std::uniform_real_distribution<float>(-1, 1);
    auto random = std::random_device();

    for (auto& boid : _boids) {
        boid.position() = {
            distribution(random),
            distribution(random),
            distribution(random)
        };
        boid.velocity() = {
            distribution(random),
            distribution(random),
            distribution(random)
        };
    }
}

void BoidSpace::update(const TotoGL::Seconds& delta) {
    for (auto& boid : _boids) {
        static constexpr auto too_close_radius = .125;
        static constexpr auto quite_close_radius = .25;
        auto too_close = std::vector<Boid>();
        auto quite_close = std::vector<Boid>();

        std::copy_if(
            _boids.begin(), _boids.end(), std::back_inserter(too_close),
            [&](Boid& other_boid) {
                return &boid != &other_boid && glm::distance(boid.position(), other_boid.position()) < too_close_radius;
            });
        std::copy_if(
            _boids.begin(), _boids.end(), std::back_inserter(quite_close),
            [&](Boid& other_boid) {
                return &boid != &other_boid && glm::distance(boid.position(), other_boid.position()) < quite_close_radius;
            });

        boid.velocity() += //
            boid.separation(too_close) + //
            boid.alignment(quite_close) + //
            boid.cohesion(quite_close);

        boid.updatePosition(delta);

        if (boid.position().x > 1)
            boid.position().x -= 2;
        if (boid.position().y > 1)
            boid.position().y -= 2;
        if (boid.position().z > 1)
            boid.position().z -= 2;
        if (boid.position().x < -1)
            boid.position().x += 2;
        if (boid.position().y < -1)
            boid.position().y += 2;
        if (boid.position().z < -1)
            boid.position().z += 2;
    }
}

void BoidSpace::render(TotoGL::Renderer& renderer, TotoGL::Camera& camera) {
    auto& object = TotoGL::RenderObjectFactory::get(_object_instance);
    for (const auto& boid : _boids) {
        object.position() = boid.position();
        object.lookAt(boid.position() + boid.velocity());
        renderer.render(object, camera);
    }
}
