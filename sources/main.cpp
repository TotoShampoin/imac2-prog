#include "prog/BoidSpace.hpp"
#include <TotoGL/TotoGL.hpp>

#include <imgui.h>
#include <tuple>

// TODO: Move all of this out of main

constexpr auto WIDTH = 1024;
constexpr auto HEIGHT = 768;
constexpr auto FOV = glm::radians(70.);
constexpr auto NEAR = .1;
constexpr auto FAR = 100.;

TotoGL::RenderObjectFactory::ObjectInstanceId createObjectInstance();
std::tuple<TotoGL::Camera, TotoGL::OrbitControl> createCameraAndOrbit();

int main(int argc, const char* argv[]) {
    // Init video
    using TotoGL::VectorEventName::WINDOW_SIZE;
    auto window = TotoGL::Window(WIDTH, HEIGHT, "Boids IMAC");
    auto renderer = TotoGL::Renderer();
    auto clock = TotoGL::Clock();
    auto [camera, orbit] = createCameraAndOrbit();
    orbit.bindEvents(window);
    const auto object_instance_id = createObjectInstance();
    window.on(WINDOW_SIZE, [&](const TotoGL::VectorEvent& event) {
        glViewport(0, 0, event.x, event.y);
        camera.setPersective(FOV, event.x / event.y, NEAR, FAR);
    });
    TotoGL::initImGui(window);

    // Init logic
    auto space = BoidSpace(75, object_instance_id);
    bool spy = false;
    size_t spy_index = 0;

    // Main loop
    while (!window.shouldClose()) {
        auto delta = clock.getDeltaTime();
        space.update(delta);

        if (spy) {
            orbit.position() = space.boids()[spy_index].position();
        }
        orbit.apply(camera);

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
                ImGui::Begin("Info");
                ImGui::Text("fps = %f", 1 / (delta ? delta : 1e-6));
                ImGui::Text("avg velocity = %f, %f, %f", avg_velocity.x, avg_velocity.y, avg_velocity.z);
                ImGui::Text("closeness 0 and 1 = %f", closeness_0_and_1);
                ImGui::End();

                ImGui::Begin("Controls");
                ImGui::SliderFloat("Avoid", &space.avoidFactor(), 0, 1);
                ImGui::SliderFloat("Match", &space.matchingFactor(), 0, 1);
                ImGui::SliderFloat("Center", &space.centeringFactor(), 0, 1);
                ImGui::SliderFloat("Cube radius", &space.cubeRadius(), 0, 10);
                static int amount = space.boids().size();
                if (ImGui::SliderInt("Amount", &amount, 0, 500)) {
                    space.resize(amount);
                }
                if (ImGui::Button("Reset")) {
                    space.resetBoids();
                }
                ImGui::End();

                ImGui::Begin("Spy");
                ImGui::Checkbox("Spy", &spy);
                if (spy) {
                    if (ImGui::Button("-")) {
                        spy_index = (spy_index + space.boids().size() - 1) % space.boids().size();
                    }
                    ImGui::SameLine();
                    ImGui::Text("%zu", spy_index);
                    ImGui::SameLine();
                    if (ImGui::Button("+")) {
                        spy_index = (spy_index + 1) % space.boids().size();
                    }

                    const auto& boid = space.boids()[spy_index];
                    ImGui::Text("Position = %f, %f, %f", boid.position().x, boid.position().y, boid.position().z);
                    ImGui::Text("Velocity = %f, %f, %f", boid.velocity().x, boid.velocity().y, boid.velocity().z);
                }
                ImGui::End();
            });
        });
    }

    return 0;
}

TotoGL::RenderObjectFactory::ObjectInstanceId createObjectInstance() {
    const auto object_instance = TotoGL::RenderObjectFactory::create(TotoGL::RenderObject(
        TotoGL::MeshFactory::create(
            TotoGL::Mesh::cube()),
        TotoGL::ShaderMaterialFactory::create(TotoGL::ShaderMaterial(
            TotoGL::VertexShader(std::ifstream("assets/shaders/shader.vert")),
            TotoGL::FragmentShader(std::ifstream("assets/shaders/shader.frag"))))));

    auto& object = TotoGL::RenderObjectFactory::get(object_instance);
    object.material().uniform("u_color", glm::vec4(1, 1, 1, 1));

    return object_instance;
}
std::tuple<TotoGL::Camera, TotoGL::OrbitControl> createCameraAndOrbit() {
    auto camera = TotoGL::Camera::Perspective(FOV, (float)WIDTH / HEIGHT, NEAR, FAR);
    camera.position() = { 2, 2, 2 };
    camera.lookAt({ 0, 0, 0 });

    auto orbit = TotoGL::OrbitControl(-glm::pi<float>() / 6, glm::pi<float>() / 4, 10);
    return { camera, orbit };
}
