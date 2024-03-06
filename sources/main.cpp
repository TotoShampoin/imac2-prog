#include "prog/BoidSpace.hpp"
#include <TotoGL/TotoGL.hpp>
#include <fstream>

#include <imgui.h>

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

constexpr auto WIDTH = 1024;
constexpr auto HEIGHT = 768;
constexpr auto FOV = glm::radians(70.);
constexpr auto NEAR = .1;
constexpr auto FAR = 100.;

using TotoGL::VectorEventName::WINDOW_SIZE;

int main(int argc, const char* argv[]) {
    auto window = TotoGL::Window(WIDTH, HEIGHT, "Boids IMAC");
    auto renderer = TotoGL::Renderer();

    const auto object_instance = TotoGL::RenderObjectFactory::create(TotoGL::RenderObject(
        TotoGL::MeshFactory::create(
            TotoGL::Mesh::cube()),
        TotoGL::ShaderMaterialFactory::create(TotoGL::ShaderMaterial(
            TotoGL::VertexShader(std::ifstream("assets/shaders/shader.vert")),
            TotoGL::FragmentShader(std::ifstream("assets/shaders/shader.frag"))))));

    auto& object = TotoGL::RenderObjectFactory::get(object_instance);
    object.material().uniform("u_color", glm::vec4(1, 1, 1, 1));

    auto camera = TotoGL::Camera::Perspective(FOV, (float)WIDTH / HEIGHT, NEAR, FAR);
    camera.position() = { 2, 2, 2 };
    camera.lookAt({ 0, 0, 0 });

    auto orbit = TotoGL::OrbitControl(0, 0, 3);
    orbit.bindEvents(window);

    auto space = BoidSpace(48, object_instance);

    auto clock = TotoGL::Clock();

    window.on(WINDOW_SIZE, [&](const TotoGL::VectorEvent& event) {
        glViewport(0, 0, event.x, event.y);
        camera.setPersective(FOV, event.x / event.y, NEAR, FAR);
    });

    TotoGL::initImGui(window);
    while (!window.shouldClose()) {
        auto delta = clock.getDeltaTime();
        orbit.apply(camera);
        space.update(delta);

        glm::vec3 avg_velocity = glm::vec3(0);
        for (const auto& boid : space.boids()) {
            avg_velocity += boid.velocity();
        }
        avg_velocity /= space.boids().size();

        float closeness_0_and_1 = space.boids()[0].closeness(space.boids()[1], .25, .25);

        window.draw([&]() {
            renderer.clear();

            space.render(renderer, camera);

            TotoGL::renderImGui([&]() {
                ImGui::Begin("Boids");
                ImGui::Text("avg velocity = %f, %f, %f", avg_velocity.x, avg_velocity.y, avg_velocity.z);
                ImGui::Text("closeness 0 and 1 = %f", closeness_0_and_1);
                ImGui::End();
            });
        });
    }

    return 0;
}
