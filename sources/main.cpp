#include "prog/Boid.hpp"
#include <TotoGL/TotoGL.hpp>
#include <algorithm>
#include <fstream>
#include <iterator>
#include <random>
#include <vector>

constexpr auto WIDTH = 1024;
constexpr auto HEIGHT = 768;
constexpr auto FOV = glm::radians(70.);
constexpr auto NEAR = .1;
constexpr auto FAR = 100.;

int main(int argc, const char* argv[]) {
    auto window = TotoGL::Window(WIDTH, HEIGHT, "Boids IMAC");
    auto renderer = TotoGL::Renderer();

    const auto mesh_instance = TotoGL::MeshFactory::create(
        TotoGL::Mesh::cube());
    const auto material_instance = TotoGL::ShaderMaterialFactory::create(
        TotoGL::ShaderMaterial(
            TotoGL::VertexShader(std::ifstream("assets/shaders/shader.vert")),
            TotoGL::FragmentShader(std::ifstream("assets/shaders/shader.frag"))));

    const auto object_instance = TotoGL::RenderObjectFactory::create(
        TotoGL::RenderObject(mesh_instance, material_instance));

    auto& object = TotoGL::RenderObjectFactory::get(object_instance);

    object.material().uniform("u_color", glm::vec4(1, 0, 0, 1));

    auto camera = TotoGL::Camera::Perspective(FOV, (float)WIDTH / HEIGHT, NEAR, FAR);

    auto orbit = TotoGL::OrbitControl(0, 0, 4);

    auto boids = std::vector<Boid>(48);
    auto distribution = std::uniform_real_distribution<float>(-1, 1);
    auto random = std::random_device();

    for (auto& boid : boids) {
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

    auto clock = TotoGL::Clock();

    object.scaling() = { .05, .05, .05 };
    while (!window.shouldClose()) {
        auto delta = clock.getDeltaTime();
        orbit.apply(camera);
        orbit.angle_y() += delta;
        window.draw([&]() {
            renderer.clear();
            // renderer.render(object, camera);

            for (auto& boid : boids) {
                static constexpr auto too_close_radius = .125;
                static constexpr auto quite_close_radius = .25;
                auto too_close = std::vector<Boid>();
                auto quite_close = std::vector<Boid>();

                std::copy_if(
                    boids.begin(), boids.end(), std::back_inserter(too_close),
                    [&](Boid& other_boid) {
                        return &boid != &other_boid && glm::distance(boid.position(), other_boid.position()) < too_close_radius;
                    });
                std::copy_if(
                    boids.begin(), boids.end(), std::back_inserter(quite_close),
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

                object.position() = boid.position();
                object.lookAt(boid.position() + boid.velocity());
                renderer.render(object, camera);
            }
        });
    }

    return 0;
}
