#include "prog/BoidSpace.hpp"
#include <algorithm>
#include <cmath>
#include <glm/geometric.hpp>
#include <glm/glm.hpp>
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
    static constexpr float too_close_radius = .125;
    static constexpr float quite_close_radius = .25;
    static constexpr float steer_angle_per_second = glm::radians(120.);
    static constexpr float returning_velocity = 1.;
    static constexpr float max_velocity = 2.;

    for (auto& boid : _boids) {
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

        // Attract back
        if (boid.position().x < -1.)
            boid.velocity().x = boid.velocity().x + delta * returning_velocity;
        if (boid.position().x > 1.)
            boid.velocity().x = boid.velocity().x - delta * returning_velocity;
        if (boid.position().y < -1.)
            boid.velocity().y = boid.velocity().y + delta * returning_velocity;
        if (boid.position().y > 1.)
            boid.velocity().y = boid.velocity().y - delta * returning_velocity;
        if (boid.position().z < -1.)
            boid.velocity().z = boid.velocity().z + delta * returning_velocity;
        if (boid.position().z > 1.)
            boid.velocity().z = boid.velocity().z - delta * returning_velocity;

        // // Bounce
        // if (boid.position().x > 1 || boid.position().x < -1)
        //     boid.velocity().x *= -1;
        // if (boid.position().y > 1 || boid.position().y < -1)
        //     boid.velocity().y *= -1;
        // if (boid.position().z > 1 || boid.position().z < -1)
        //     boid.velocity().z *= -1;

        // // Wrap around
        // if (boid.position().x > 1)
        //     boid.position().x -= 2;
        // if (boid.position().y > 1)
        //     boid.position().y -= 2;
        // if (boid.position().z > 1)
        //     boid.position().z -= 2;
        // if (boid.position().x < -1)
        //     boid.position().x += 2;
        // if (boid.position().y < -1)
        //     boid.position().y += 2;
        // if (boid.position().z < -1)
        //     boid.position().z += 2;

        if (glm::length(boid.velocity()) > max_velocity)
            boid.velocity() = glm::normalize(boid.velocity()) * max_velocity;

        boid.updatePosition(delta);
    }
}

void BoidSpace::render(TotoGL::Renderer& renderer, TotoGL::Camera& camera) {
    auto& object = TotoGL::RenderObjectFactory::get(_object_instance);

    object.material().uniform("u_color", glm::vec4(.5, .5, 0., 1.));
    object.mesh().cull_face() = TotoGL::Mesh::CullFace::BACK;
    object.scaling() = { 2, 2, 2 };
    object.position() = { 0, 0, 0 };
    object.rotation() = { 0, 0, 0 };
    renderer.render(object, camera);

    glClear(GL_DEPTH_BUFFER_BIT);

    object.material().uniform("u_color", glm::vec4(1., 1., 1., 1.));
    object.mesh().cull_face() = TotoGL::Mesh::CullFace::FRONT;
    object.scaling() = { .05, .05, .05 };
    for (const auto& boid : _boids) {
        object.position() = boid.position();
        object.lookAt(boid.position() + boid.velocity());
        renderer.render(object, camera);
    }

    object.material().uniform("u_color", glm::vec4(.5, .5, 0., .25));
    object.mesh().cull_face() = TotoGL::Mesh::CullFace::BACK;
    object.scaling() = { 2, 2, 2 };
    object.position() = { 0, 0, 0 };
    object.rotation() = { 0, 0, 0 };
    renderer.render(object, camera);
}
