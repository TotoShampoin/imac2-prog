#include "SecondOrderDynamics.hpp"
#include "prog/BoidSpace.hpp"
#include "prog/imgui-impl.hpp"
#include <TotoGL/TotoGL.hpp>

#include <glm/ext/scalar_constants.hpp>
#include <glm/fwd.hpp>
#include <imgui.h>
#include <tuple>

// TODO: Move all of this out of main

constexpr auto WIDTH = 1024;
constexpr auto HEIGHT = 768;
constexpr auto FOV = glm::radians(70.);
constexpr auto NEAR = .1;
constexpr auto FAR = 100.;

TotoGL::RenderObjectInstanceId createObjectInstance();
std::tuple<TotoGL::Camera, TotoGL::OrbitControl> createCameraAndOrbit();

int main(int argc, const char* argv[]) {
    // Init video
    using TotoGL::VectorEventName::WINDOW_SIZE;
    auto window = TotoGL::Window(WIDTH, HEIGHT, "Boids IMAC");
    auto renderer = TotoGL::Renderer();
    auto clock = TotoGL::Clock();
    auto [camera, orbit] = createCameraAndOrbit();
    orbit.bindEvents(window, [&]() { return isImGuiFocused(); });
    const auto object_instance_id = createObjectInstance();
    window.on(WINDOW_SIZE, [&](const TotoGL::VectorEvent& event) {
        glViewport(0, 0, event.x, event.y);
        camera.setPersective(FOV, event.x / event.y, NEAR, FAR);
    });
    // TotoGL::initImGui(window);
    initImGui(window);

    auto camera_target = camera;
    auto position_dynamic = SecondOrderDynamics(1, 1, 0, camera.position());
    auto rotation_dynamic = SecondOrderDynamics(1, 1, 0, camera.rotation());
    const auto update_camera = [&](float delta) {
        camera.position() = position_dynamic.update(delta, camera_target.position());
        // the target's rotation is an euler angle, so we need to account for the fact that the angle wraps around.
        // That wrapping around is made automatically by the camera, so by default, the dynamics does a funky movement when the angle wraps around.
        // To fix that, we need to undo the wrapping around by adding or subtracting 2pi to the target's rotation.
        auto target_rotation = camera_target.rotation();
        auto current_rotation = camera.rotation();
        for (int i = 0; i < 3; i++) {
            if (target_rotation[i] - current_rotation[i] > glm::pi<float>()) {
                target_rotation[i] -= glm::two_pi<float>();
            } else if (current_rotation[i] - target_rotation[i] > glm::pi<float>()) {
                target_rotation[i] += glm::two_pi<float>();
            }
        }
        camera.rotation() = rotation_dynamic.update(delta, target_rotation);
    };

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
        orbit.apply(camera_target);
        update_camera(delta);

        window.draw([&]() {
            renderer.clear();

            space.render(renderer, camera);

            renderImGui([&]() {
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
                if (ImGui::Checkbox("Spy", &spy)) {
                    if (spy) {
                        orbit.distance() = 1;
                    } else {
                        orbit.distance() = 10;
                        orbit.position() = { 0, 0, 0 };
                    }
                }
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

TotoGL::RenderObjectInstanceId createObjectInstance() {
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
