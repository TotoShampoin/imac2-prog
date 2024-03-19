#include "data.hpp"

#include "TotoGL/CameraControl/OrbitControl.hpp"
#include "TotoGL/RenderObject/Camera.hpp"
#include "prog/BoidContainer.hpp"
#include "prog/imgui-impl.hpp"

#include <TotoGL/TotoGL.hpp>
#include <imgui.h>

Data::Data(TotoGL::Window& window)
    : window(window)
    , camera(TotoGL::Camera::Perspective(FOV, (float)WIDTH / HEIGHT, NEAR, FAR))
    , orbit(-glm::pi<float>() / 6, glm::pi<float>() / 4, 10)
    , container(75)
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

void Data::update(const TotoGL::Seconds& delta) {
    container.update(delta);
    if (spy) {
        orbit.position() = container.boids()[spy_index].position();
    }
    orbit.apply(camera);

    if (changing_amount) {
        container.resize(amount);
        changing_amount = false;
    }
    if (resetting) {
        container.resetBoids();
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
        spy_index = (spy_index + 1) % container.boids().size();
        spying_next = false;
    }
    if (spying_previous) {
        spy_index = (spy_index + container.boids().size() - 1) % container.boids().size();
        spying_previous = false;
    }
}

void Data::draw() {
    window.draw([&]() {
        renderer.clear();
        // Boid rendering
        if (spy)
            boid_scene.draw(renderer, camera, container, container.boids()[spy_index]);
        else
            boid_scene.draw(renderer, camera, container);

        // UI rendering
        renderImGui([&]() {
            ImGui::Begin("Controls");
            ImGui::SliderFloat("Avoid", &container.avoidFactor(), 0, 1);
            ImGui::SliderFloat("Match", &container.matchingFactor(), 0, 1);
            ImGui::SliderFloat("Center", &container.centeringFactor(), 0, 1);
            ImGui::SliderFloat("Cube radius", &container.cubeRadius(), 0, 10);
            ImGui::SliderFloat("Attract radius", &container.attractRadius(), 0, 1);
            ImGui::SliderFloat("Expell radius", &container.expellRadius(), 0, 1);
            ImGui::SliderFloat("Returning velocity", &container.returningVelocity(), 0, 10);
            ImGui::SliderFloat("Max velocity", &container.maxVelocity(), 0, 10);
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

                const auto& boid = container.boids()[spy_index];
                ImGui::Text("Position = %f, %f, %f", boid.position().x, boid.position().y, boid.position().z);
                ImGui::Text("Velocity = %f, %f, %f", boid.velocity().x, boid.velocity().y, boid.velocity().z);
            }
            ImGui::End();
        });
    });
}
