#include "data.hpp"

#include "TotoGL/CameraControl/OrbitControl.hpp"
#include "TotoGL/RenderObject/Camera.hpp"
#include "prog/BoidSpace.hpp"
#include "prog/imgui-impl.hpp"

#include <TotoGL/TotoGL.hpp>
#include <imgui.h>

TotoGL::RenderObjectInstanceId createObjectInstance() {
    return TotoGL::RenderObjectFactory::create(TotoGL::RenderObject(
        TotoGL::MeshFactory::create(
            TotoGL::Mesh::cube()),
        TotoGL::ShaderMaterialFactory::create(TotoGL::ShaderMaterial(
            TotoGL::VertexShader(std::ifstream("assets/shaders/shader.vert")),
            TotoGL::FragmentShader(std::ifstream("assets/shaders/shader.frag"))))));
}

Data::Data(TotoGL::Window& window)
    : window(window)
    , camera(TotoGL::Camera::Perspective(FOV, (float)WIDTH / HEIGHT, NEAR, FAR))
    , orbit(-glm::pi<float>() / 6, glm::pi<float>() / 4, 10)
    , appearance(createObjectInstance())
    , boid_space(75)
    , amount(75)
    , spy(false)
    , spy_index(0) {
    window.on(TotoGL::VectorEventName::WINDOW_SIZE, [&](const TotoGL::VectorEvent& event) {
        glViewport(0, 0, event.x, event.y);
        camera.setPersective(FOV, event.x / event.y, NEAR, FAR);
    });
    orbit.bindEvents(window, isImGuiFocused);
    initImGui(window);
}

Data::~Data() {
    TotoGL::RenderObjectFactory::destroy(appearance);
}

void Data::update(const TotoGL::Seconds& delta) {
    boid_space.update(delta);
    if (spy) {
        orbit.position() = boid_space.boids()[spy_index].position();
    }
    orbit.apply(camera);

    if (changing_amount) {
        boid_space.resize(amount);
        changing_amount = false;
    }
    if (resetting) {
        boid_space.resetBoids();
        resetting = false;
    }
    if (toggling_spy) {
        if (spy) {
            orbit.distance() = 1;
        } else {
            orbit.distance() = 10;
            orbit.position() = { 0, 0, 0 };
        }
        toggling_spy = false;
    }
    if (spying_next) {
        spy_index = (spy_index + 1) % boid_space.boids().size();
        spying_next = false;
    }
    if (spying_previous) {
        spy_index = (spy_index + boid_space.boids().size() - 1) % boid_space.boids().size();
        spying_previous = false;
    }
}

void Data::draw() {
    window.draw([&]() {
        renderer.clear();
        boid_space.render(renderer, camera, *appearance);

        renderImGui([&]() {
            ImGui::Begin("Controls");
            ImGui::SliderFloat("Avoid", &boid_space.avoidFactor(), 0, 1);
            ImGui::SliderFloat("Match", &boid_space.matchingFactor(), 0, 1);
            ImGui::SliderFloat("Center", &boid_space.centeringFactor(), 0, 1);
            ImGui::SliderFloat("Cube radius", &boid_space.cubeRadius(), 0, 10);
            changing_amount |= ImGui::SliderInt("Amount", &amount, 0, 500);
            resetting |= ImGui::Button("Reset");
            ImGui::End();

            ImGui::Begin("Spy");
            toggling_spy |= ImGui::Checkbox("Spy", &spy);
            if (spy) {
                spying_previous |= ImGui::Button("-");
                ImGui::SameLine();
                ImGui::Text("%zu", spy_index);
                ImGui::SameLine();
                spying_next |= ImGui::Button("+");

                const auto& boid = boid_space.boids()[spy_index];
                ImGui::Text("Position = %f, %f, %f", boid.position().x, boid.position().y, boid.position().z);
                ImGui::Text("Velocity = %f, %f, %f", boid.velocity().x, boid.velocity().y, boid.velocity().z);
            }
            ImGui::End();
        });
    });
}
