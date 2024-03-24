#include "data.hpp"

#include "TotoGL/CameraControl/OrbitControl.hpp"
#include "TotoGL/RenderObject/Camera.hpp"
#include "TotoGL/RenderObject/Light.hpp"
#include "prog/BoidContainer.hpp"
#include "prog/imgui-impl.hpp"

#include <TotoGL/TotoGL.hpp>
#include <imgui.h>

Data::Data(TotoGL::Window& window, TotoGL::Renderer& renderer)
    : window(window)
    , renderer(renderer) {
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
    if (spy)
        boid_scene.update(container, container.boids()[spy_index]);
    else
        boid_scene.update(container);

    scene.clear();
    auto& boid_objects = boid_scene.objects();
    for (auto& object : boid_objects) {
        scene.add(object);
    }
    scene.add(ambient_light);
    scene.add(global_light);
    global_light->setDirection({ 1, -1, 1 });

    window.draw([&]() {
        renderer.clear();
        renderer.render(scene, camera);

        renderImGui([&]() {
            ImGui::SetNextWindowPos(ImVec2(0, window.size()[1]), ImGuiCond_Always, ImVec2(0, 1));
            ImGui::SetNextWindowSize(ImVec2(415, 261), ImGuiCond_Always);
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

            ImGui::SetNextWindowPos(ImVec2(415, window.size()[1]), ImGuiCond_Always, ImVec2(0, 1));
            ImGui::SetNextWindowSize(ImVec2(304, 111), ImGuiCond_Always);
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
